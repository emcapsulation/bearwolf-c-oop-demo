#include "clairvoyant.h"
#include "../player_protected.h"

#include <stdio.h>
#include <stdlib.h>


/*
* vTable Methods
*/
static void Clairvoyant_show_summary(Player* player)
{
    Default_show_summary(player);
    printf("\nYou can select one player to see their role.\n");
}

static Event Clairvoyant_special_ability(Player* self, Player* target)
{
    printf("You see Player %d's role: %s.\n", target->player_id, Util_role_to_string(target->role));
    return (Event){ .player_id = target->player_id, .action = SEE };
}

static const Player_vTable clairvoyant_vTable = {
    .show_summary = Clairvoyant_show_summary,
    .output_properties = Default_output_properties,
    .special_ability = Clairvoyant_special_ability,
    .delete = Default_dtor
};


/*
* Public
*/
Clairvoyant* Clairvoyant_ctor(const int player_id) {
    Clairvoyant* clairvoyant = malloc(sizeof(Clairvoyant));
    if (!clairvoyant) exit(EXIT_FAILURE);

    Player_init(&clairvoyant->super, &clairvoyant_vTable, player_id, CLAIRVOYANT);

    return clairvoyant;
}