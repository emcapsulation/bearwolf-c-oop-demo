#ifndef PLAYER_H
#define PLAYER_H

#include "../util/util.h"


typedef struct Player_vTable Player_vTable;
typedef struct Player_protected Player_protected;


typedef struct Player 
{
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
	int player_id;
	Action action;
} Event;

extern const Event DEFAULT_EVENT;


/*
* Public Methods
*/
Player* Player_factory(const int player_id, const Role role);
void Player_reset(Player* player);
int Player_is_alive(const Player* player);
void Player_eliminate(Player* player);
int Player_can_vote(const Player* player);
void Player_ban_vote(Player* player);
int Player_is_bitten(const Player* player);


/*
* vTable Public Methods
*/
typedef struct Player_vTable
{
	void (*show_summary)(Player* self);
	void (*output_properties)(Player* self, Player* player);
	Event(*gets_bitten)(Player* self);
	Event(*special_ability)(Player* self, Player* target);
	void (*delete)(Player* self);
} Player_vTable;

#endif