#include "townsperson.h"
#include "../player_protected.h"

#include <stdio.h>
#include <stdlib.h>


/*
* vTable Methods
*/
static void Townsperson_show_summary(Player* player)
{
    Default_show_summary(player);
    printf("\nYou have no special ability. Go back to sleep.\n");
}

static const Player_vTable townsperson_vTable = {
    .show_summary = Townsperson_show_summary,
    .output_properties = Default_output_properties,
    .gets_bitten = Default_gets_bitten,
    .special_ability = Default_special_ability,
    .delete = Default_dtor
};


/*
* Public
*/
Townsperson* Townsperson_ctor(const int player_id) {
    Townsperson* townsperson = malloc(sizeof(Townsperson));
    if (!townsperson) exit(EXIT_FAILURE);

    Player_init(&townsperson->super, &townsperson_vTable, player_id, TOWNSPERSON);

    return townsperson;
}