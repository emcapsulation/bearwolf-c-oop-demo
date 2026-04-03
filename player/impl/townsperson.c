#include "townsperson.h"
#include "../player_protected.h"

#include <stdio.h>
#include <stdlib.h>


/*
* vTable Methods
*/
static void Townsperson_show_summary(const Player* player)
{
    Default_show_summary(player);
    printf("\nYou have no special ability. Go back to sleep.\n");
}

void Townsperson_dtor(Player* self)
{
    Townsperson* townsperson = (Townsperson*)self;
    free(townsperson->super.protected);
    free(townsperson);
}

static const Player_vTable townsperson_vTable = {
    .show_summary = Townsperson_show_summary,
    .special_ability = Default_special_ability,
    .destroy = Townsperson_dtor
};


/*
* Public
*/
Townsperson* Townsperson_ctor(const int player_id) {
    Townsperson* townsperson = malloc(sizeof(Townsperson));
    if (!townsperson) exit(EXIT_FAILURE);

    super(&townsperson->super, &townsperson_vTable, player_id, TOWNSPERSON);

    return townsperson;
}