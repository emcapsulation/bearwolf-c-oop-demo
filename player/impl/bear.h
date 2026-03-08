#ifndef BEAR_H
#define BEAR_H

#include "../player_protected.h"

typedef struct Bear {
	Player super;
} Bear;

Bear* Bear_ctor(const int player_id);

#endif