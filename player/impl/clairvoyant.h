#ifndef CLAIRVOYANT_H
#define CLAIRVOYANT_H

#include "../player.h"

typedef struct Clairvoyant {
	Player super;
} Clairvoyant;

Clairvoyant* Clairvoyant_ctor(const int player_id);

#endif