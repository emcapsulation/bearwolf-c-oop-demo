#ifndef PLAYER_PROTECTED_H
#define PLAYER_PROTECTED_H

#include "player.h"


struct Player_protected {
	int is_alive;
	int is_bitten;
	int can_vote;
};

void Player_init(Player* self, Player_vTable* vTable, const int player_id, const Role role);


/**
 * Default vTable Methods
 */
void Default_show_summary(Player* self);
void Default_output_properties(Player* self, Player *player);
void Default_dtor(Player* self);

#endif