#ifndef ACTIVIST_H
#define ACTIVIST_H

#include "../player_protected.h"

typedef struct Activist_private Activist_private;

typedef struct Activist {
	Player super;
	Activist_private* private;
} Activist;

Activist* Activist_ctor(const int player_type_id);

#endif