#ifndef PLAYER_PROTECTED_H
#define PLAYER_PROTECTED_H

#include "player.h"


struct Player_protected {
	int is_alive;
	int is_bitten;
	int can_vote;
};

void Player_init(Player* self, const Player_vTable* vTable, const int player_id, const Role role);


/*
* Default vTable Methods
*/
void Default_show_summary(Player* self);
void Default_output_properties(Player* self, Player *player);
Event Default_gets_bitten(Player* self);
Event Default_special_ability(Player* self, Player* target);
void Default_dtor(Player* self);

#endif