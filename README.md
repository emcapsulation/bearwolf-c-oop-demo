# "You can't do object-oriented programming in C"
This repo is intended to be a exploration of some ways to emulate object-oriented 
programming principles in C. 

The full tutorial and explanations can be found in my 
[YouTube Video](https://www.youtube.com/watch?v=1VPx7Tz_d6A).

Note that in the video, I also evaluate the approaches and discuss some of their 
limitations. This code is provided "as is" for educational purposes! :)

# Running the Code
Nothing fancy here.
```
gcc main.c game/game.c util/util.c player/player.c player/impl/activist.c player/impl/bear.c player/impl/clairvoyant.c player/impl/healer.c player/impl/townsperson.c -o main -Wall -Werror -fsanitize=address
```
```
./main
```

# Four Pillars
## Encapsulation
Encapsulation is the principle of bundling related fields and functions associated 
with an entity together into a group, commonly known as a class. A second principle 
of encapsulation is hiding object internals so that the object can enforce that its 
internal state is accessed and altered in a controlled way, via the public methods
defined by the object itself.

### Data Grouping: Structs
We bundled data using structs, which store each member variable in a contiguous 
block of memory (grouped under a single type).
```c
struct Game {
	int num_players;
	int num_bears_left;
	int num_townspeople_left;
	int is_night;

	int player_bitten_ids[NUM_BEARS];
	int bites_this_round;
	int player_healed_id;
	Player** players;	
};
```

### Method Grouping: Header File Organisation
To group this data alongside its methods, we created corresponding .c and .h files 
for each particular entity. The .h file represents the type's public API: This is 
where function prototypes intended to be public are placed. Those functions were 
implemented fully in the corresponding .c file for that type.
```c title="game.h"
Game* Game_ctor(const int num_players);
void Game_loop(Game* self);
void Game_dtor(Game* self);
```

### Private Data: Opaque Types
Private data was emulated using opaque types. This is a struct which is forward 
declared in the type's header file, but the full struct definition is placed in the 
.c file. Then, any outside code including the .h file can only observe the 
incomplete type, preventing them from accessing internal fields or even initialising 
the struct (as the size is unknown to it). That way, all access and modifications 
to the struct internals take place through the object's public functions, whose 
prototypes are in the .h file.
```c title="game.h"
typedef struct Game Game;
```
```c title="game.c"
struct Game {
	int num_players;
	int num_bears_left;
	int num_townspeople_left;
	int is_night;

	int player_bitten_ids[NUM_BEARS];
	int bites_this_round;
	int player_healed_id;
	Player** players;	
};
```

### Private Methods: Static Keyword
We emulated private methods by simply placing their definitions in the .c file, 
without putting their prototypes in the .h file. Unfortunately, external code can 
still forward declare the function and use it, so a nice way to prevent this is to 
mark private functions as static to give them internal linkage. That is, the symbol 
is only visible to that translation unit.
```c title="game.c"
static void Game_init_round_properties(Game* self)
{
	for (int i = 0; i < NUM_BEARS; i++)
		self->player_bitten_ids[i] = -1;
	self->bites_this_round = 0;
	self->player_healed_id = -1;
}
```

## Abstraction
Abstraction is the concept of presenting a simple interface for others to interact 
with your class type most effectively, whilst hiding the internal complexity 
associated with the type's operations.

A nice feature in some languages are abstract classes - these present an interface 
through a set of abstract methods which must be implemented by the concrete subtypes 
which extend it. Thus the class cannot be instantiated directly. It may also include 
fully implemented methods which are common to the subtypes (but may be overridden).

### Abstract Classes: Base Type Initialisation Function
To emulate an abstract class in C, we did not give the Player type a public 
constructor (to discourage its instantiation). Instead, we gave it an initialisation 
function, which is akin to a base class constructor in that it does not allocate 
memory for the base type - it merely accepts a pointer to an already-constructed 
concrete type and initialises the member variables common to those subtypes.
```c title="player.c"
void super(Player* self, const Player_vTable* vTable, const int player_id, const Role role)
{
    self->vTable = vTable;
    self->player_id = player_id;
    self->role = role;
    self->protected = Player_protected_ctor();
}
```

Then concrete subtypes can simply invoke the super() base type constructor in their 
public constructors to initialise the fields associated with the base type.
```c title="bear.c"
Bear* Bear_ctor(const int player_id) 
{
    Bear* bear = malloc(sizeof(Bear));
    if (!bear) exit(EXIT_FAILURE);

    super((Player*)bear, &bear_vTable, player_id, BEAR);

    return bear;
}
```

### Protected Visibility: PImpl Idiom and Opaque Pointers
Protected visibility refers to only allowing subtypes access to member variables and 
methods, and no other types. We simulated this by including data and function 
prototypes of methods intended to be protected in a second header file for the 
Player type: player_protected.h. This is to be included in the .c files of only the 
Player subtypes.
```c title="player_protected.h"
struct Player_protected {
	int is_alive;
	int is_bitten;
	int can_vote;
};

void super(Player* self, const Player_vTable* vTable, const int player_id, const Role role);
```

As for the struct data itself, protected member variables were mocked using opaque 
pointers. A forward declaration to the Player_protected struct (which defines the 
protected data) was placed in the player.h header. This struct was defined properly 
in the player_protected.h file. The Player struct then contained a pointer to this 
implementation. This is imilar in practise to the PImpl idiom in C++.
```c title="player.h"
typedef struct Player_protected Player_protected;

typedef struct Player {
	const Player_vTable* vTable;
	int player_id;
	Role role;
	Player_protected* protected;	
} Player;
```

## Inheritance
Inheritance is the concept of subtypes building on the base type, both taking on its 
member variables and methods, and extending its functionality.

### Upcasting: Struct Embedding
We mocked this here by embedding the Player base type as the first struct member of 
the concrete subtype structs. This allows for upcasting: Casting a pointer to the 
concrete subtype back to a pointer to a Player, because the underlying memory at the 
address of the subtype is the same as a Player type (since it's located at the start 
of the memory block).

Any private data of the type can be included in the struct using the opaque pointer 
technique, and must be placed after the Player nested struct.
```c title="activist.h"
typedef struct Activist_private Activist_private;

typedef struct Activist {
	Player super;
	Activist_private* private;
} Activist;
```

### Extensibility and Downcasting: Type Information
Any additional methods of the subtypes can have their function prototypes placed in 
the subtypes' header files, as usual. However, these should accept a pointer to the 
subtype (not the base Player type) to show it is a method applicable to that subtype 
only.

Therefore, we supported downcasting: Casting a pointer to the Player base type back 
to the narrower subtype. As C has no automatic runtime type information, subtypes 
must carry their type information through a field - we achieved this here with the 
role member variable. This must be explicitly checked before downcasting.
```c title="game.c"
if (eliminated->role == HEALER && Healer_attempt_self_heal((Healer*)eliminated))
	return;
```

## Polymorphism
Polymorphism is the concept of different subtypes existing under a shared interface, 
who respond to a common interface in different ways.

### Dynamic Method Dispatch: vTables and vPtrs
We examined a form of runtime polymorphism: Dynamic method dispatch. This is when 
subtypes respond to the same function call differently, as the correct method 
implementation is invoked at runtime depending on the object's type.

We implemented this using vTables. A vTable is a struct of function pointers to the 
methods which must be implemented fully by the subtypes.
```c
typedef struct Player_vTable {
	void (*show_summary)(const Player* self);
	Event(*special_ability)(Player* self, Player* target);
	void (*destroy)(Player* self);
} Player_vTable;
```

The base Player type contains a pointer to a Player_vTable, known as its vPtr.
```c title="player.h"
typedef struct Player {
	const Player_vTable* vTable;
	int player_id;
	Role role;
	Player_protected* protected;	
} Player;
```

Each subtype then implements all methods described in the Player_vTable. We gave 
them each their own Player_vTable instance, with static storage duration, whose 
members point to the desired function implementations for that subtype.
```c title="bear.h"
static const Player_vTable bear_vTable = {
    .show_summary = Bear_show_summary,
    .special_ability = Bear_special_ability,    
    .destroy = Bear_dtor
};
```

Importantly, we then hooked the subtype's vTable to its vPtr during construction, by 
passing the address of that subtype's vTable instance to the Player initialisation 
function.
```c title="bear.c"
Bear* Bear_ctor(const int player_id) 
{
    Bear* bear = malloc(sizeof(Bear));
    if (!bear) exit(EXIT_FAILURE);

    super((Player*)bear, &bear_vTable, player_id, BEAR);

    return bear;
}
```

# Thank You
I hope you enjoy the video and code, and that these tips were useful for you! :)