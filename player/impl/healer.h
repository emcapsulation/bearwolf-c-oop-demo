#ifndef HEALER_H
#define HEALER_H

#include "../player_protected.h"

typedef struct Healer_private Healer_private;

typedef struct Healer {
	Player super;
	Healer_private* private;
} Healer;

Healer* Healer_ctor(const int player_id);
int Healer_attempt_self_heal(Healer* self);

#endif