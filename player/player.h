#ifndef PLAYER_H
#define PLAYER_H

#include "../game/game.h"


typedef struct Player_vTable Player_vTable;
typedef struct Player_protected Player_protected;
typedef char Role[16];


typedef struct Player 
{
	const Player_vTable* vTable;
	int player_id;
	Role role;
	Player_protected* protected;	
} Player;


typedef struct Player_vTable 
{
	void (*show_summary)(Player* self);
	void (*output_properties)(Player *self, Player* player);
	int (*special_ability)(Player* self, Player* target, Game_context *context);
    void (*delete)(Player* self);
} Player_vTable;


Player* Player_factory(const int player_id, const Role role);
void Player_reset(Player* player);
int Player_is_alive(const Player* player);
void Player_eliminate(Player* player);
int Player_can_vote(const Player* player);


/**
 * vTable Public Methods
 */
void Player_show_summary(Player* self);
void Player_output_properties(Player* self, Player* player);
int Player_special_ability(Player* self, Player *target, Game_context *context);
void delete(Player* self);

#endif