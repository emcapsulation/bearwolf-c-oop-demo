#ifndef BEAR_H
#define BEAR_H

#include "../player_protected.h"

typedef struct Bear {
	Player super;
} Bear;

Bear* Bear_ctor(const int player_id);
void Bear_output_player(const Player* output_player);

#endif