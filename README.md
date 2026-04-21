# "You can't do object-oriented programming in C"
This repo is intended to be a demonstration of emulating object-oriented programming principles in C. 

# Running the Code
```
gcc main.c game/game.c util/util.c player/player.c player/impl/activist.c player/impl/bear.c player/impl/clairvoyant.c player/impl/healer.c player/impl/townsperson.c -o main -Wall -Werror -fsanitize=address

./main
```

# Four Pillars
## Encapsulation
Encapsulation is the principle of bundling related fields and functions associated with an entity together into a group, commonly known as a class. Another principle of encapsulation is hiding object internals so that the object can allow its internal state to be altered and accessed in a controlled way, via public methods.

We bundled data using structs, which store each member variable in a contiguous block of memory (grouped under a single type).
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

Then to group this data alongside its methods, we created corresponding .c and .h files for each particular type. The .h file represents the object's public API: This is where function prototypes intended to be public are placed. Those functions were implemented fully in the corresponding .c file for the "class".
```c title="game.h"
Game* Game_ctor(const int num_players);
void Game_loop(Game* self);
void Game_dtor(Game* self);
```

Private data was emulated using opaque types. The struct is forward declared in the header file. The full struct definition is placed in the .c file for that object. Then, any outside code including the .h file only has access to the incomplete type, so they cannot access internal fields or even initialise the struct (as the size is unknown to it). That way, all access / modifications to the struct internals happen through the object's public functions, whose prototypes are in the .h file.
```c title="game.h"
/*
* Encapsulation: Game is an opaque type.
*/
typedef struct Game Game;
```
game.c
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

We emulated private methods by simply placing their definitions in the .c file, but not putting their prototypes in the .h file. Unfortunately, external code can still forward declare the function and use it, so a nice way to prevent this is to mark private functions as static to give them internal linkage. That is, the symbol is only visible to that translation unit.
```c title="game.c"
// Static keyword for internal linkage
static void Game_init_round_properties(Game* self)
{
	for (int i = 0; i < NUM_BEARS; i++)
		self->player_bitten_ids[i] = -1;
	self->bites_this_round = 0;
	self->player_healed_id = -1;
}
```

## Abstraction
Abstraction is the concept of presenting a simple interface for others to interact with your class type, hiding internal complexity behind the type's operations.

A nice feature in some languages which abstracts away an interface from the underlying details is an abstract class. An abstract class cannot be instantiated directly; rather it presents a set of abstract methods to be implemented by the concrete subtypes. It may also include fully implemented methods which are common to the subtypes (but may be overridden).

To emulate an abstract class in C, we did not give the type a constructor (to discourage its instantiation). Instead, we gave it an initialisation function, which is akin to a base class constructor in that it does not allocate the base type, it merely accepts a pointer to a constructed concrete type and initialises the member variables common to all subtypes.
```c title="player.c"
void super(Player* self, const Player_vTable* vTable, const int player_id, const Role role)
{
    self->vTable = vTable;
    self->player_id = player_id;
    self->role = role;
    self->protected = Player_protected_ctor();
}
```

Then concrete subtypes can invoke the super() base type constructor in their public constructors.
```c title="bear.c"
Bear* Bear_ctor(const int player_id) 
{
    Bear* bear = malloc(sizeof(Bear));
    if (!bear) exit(EXIT_FAILURE);

    super((Player*)bear, &bear_vTable, player_id, BEAR);

    return bear;
}
```

Protected visibility refers to only allowing subtypes access to member variables and methods, and no other types.

We simulated this by including data and function prototypes of methods intended to be protected in a second header file for the Player type: player_protected.h. This is to be included only by subtypes of Player.
```c title="player_protected.h"
struct Player_protected {
	int is_alive;
	int is_bitten;
	int can_vote;
};

void super(Player* self, const Player_vTable* vTable, const int player_id, const Role role);
```

As for the struct data itself, protected member variables were mocked using opaque pointers. A forward declaration to the Player_protected struct (which contains the protected data) was placed in the player.h header. This was defined properly in the player_protected.h file. The Player struct then contained a pointer to this implementation. This kind of reminded me of the PImpl idiom in C++.
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
Inheritance is the concept of subtypes building on the base type, both taking on its member variables and methods, and extending its functionality.

We mocked this here by embedding the Player base type as the first struct member of the concrete subtype structs. This allows for upcasting: Casting the concrete subtypes back to a Player, because the underlying memory at the address of the subtype is the same as a Player type (since it's located at the start of the memory block).

Any private data of the type can be included in the struct using the opaque pointer technique, and must be placed after the Player nested struct.
```c title="activist.h"
typedef struct Activist_private Activist_private;

typedef struct Activist {
	Player super;
	Activist_private* private;
} Activist;
```

Any additional methods of the subtypes can have their function prototypes placed in the subtypes' header files, as usual. However, these should accept a pointer to the subtype (not the base Player type) to show it is a method applicable to that subtype only.

Therefore, we supported downcasting: Casting a Player to the narrower subtype. As C has no automatic runtime type information, subtypes must carry their type information through a field - we achieved this here with the role member variable. This must be explicitly checked before downcasting.
```c title="game.c"
if (eliminated->role == HEALER && Healer_attempt_self_heal((Healer*)eliminated))
	return;
```

## Polymorphism
Polymorphism is the idea of different subtypes existing under a shared interface, who respond to common actions in different ways.

We examined a form of runtime polymorphism: Dynamic method dispatch. This is when subtypes respond to the same function call differently, as the correct method implementation is invoked at runtime depending on the object's type.

We implemented this using vTables. A vTable is a struct of function pointers to the methods which must be implemented fully by the subtypes.
```c
typedef struct Player_vTable {
	void (*show_summary)(const Player* self);
	Event(*special_ability)(Player* self, Player* target);
	void (*destroy)(Player* self);
} Player_vTable;
```

The base Player type contains a pointer to a Player_vTable, known as its v-pointer.
```c title="player.h"
typedef struct Player {
	const Player_vTable* vTable;
	int player_id;
	Role role;
	Player_protected* protected;	
} Player;
```

Each subtype then implements all methods described in the Player_vTable. We gave them each their own Player_vTable instance, with static storage duration, whose members point to the implementations for that subtype.
```c title="bear.h"
static const Player_vTable bear_vTable = {
    .show_summary = Bear_show_summary,
    .special_ability = Bear_special_ability,    
    .destroy = Bear_dtor
};
```

Importantly, we then hooked the subtype's vTable to its vPtr during construction, by passing the address of that subtype's vTable instance to the Player initialisation function.
```c title="bear.c"
Bear* Bear_ctor(const int player_id) 
{
    Bear* bear = malloc(sizeof(Bear));
    if (!bear) exit(EXIT_FAILURE);

    super((Player*)bear, &bear_vTable, player_id, BEAR);

    return bear;
}
```