#ifndef PLAYER_H
#define PLAYER_H


typedef enum {
	BEAR,
	ACTIVIST,
	CLAIRVOYANT,
	HEALER,
	TOWNSPERSON,
	ROLE_COUNT
} Role;


typedef struct Player_vTable Player_vTable;
typedef struct Player_protected Player_protected;


typedef struct Player {
	const Player_vTable* vTable;
	int player_id;
	Role role;
	Player_protected* protected;	
} Player;


typedef enum Action {
	NO_ACTION,
	BITE,
	BAN_VOTE,
	SEE,
	HEAL
} Action;

typedef struct Event {
	int target_player_id;
	Action action;
} Event;

extern const Event DEFAULT_EVENT;


/*
* Public Methods
*/
Player* Player_factory(const int player_id, const Role role);
void Player_reset(Player* self);
int Player_is_alive(const Player* self);
void Player_eliminate(Player* self);
int Player_can_vote(const Player* self);
int Player_ban_vote(Player* self);
int Player_is_bitten(const Player* self);
int Player_gets_bitten(Player* self);

// Does not act on a Player instance (static method)
const char* Player_role_to_string(Role role);


/*
* vTable Public Methods
*/
typedef struct Player_vTable {
	void (*show_summary)(const Player* self);
	Event(*special_ability)(Player* self, Player* target);
	void (*destroy)(Player* self);
} Player_vTable;

#endif