#ifndef PLAYER_PROTECTED_H
#define PLAYER_PROTECTED_H

#include "player.h"


struct Player_protected {
	int is_alive;
	int is_bitten;
	int can_vote;
};

void super(Player* self, const Player_vTable* vTable, const int player_id, const Role role);


/*
* Default vTable Methods
*/
void Default_show_summary(const Player* self);
Event Default_special_ability(Player* self, Player* target);

#endif