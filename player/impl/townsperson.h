#ifndef TOWNSPERSON_H
#define TOWNSPERSON_H

#include "../player_protected.h"

typedef struct Townsperson {
	Player super;
} Townsperson;

Townsperson* Townsperson_ctor(const int player_id);

#endif