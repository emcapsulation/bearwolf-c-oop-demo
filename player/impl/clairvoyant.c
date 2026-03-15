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
    printf("You see Player %d's role: %s.\n", target->player_id, target->role);
    return (Event){ .player_id = target->player_id, .action = SEE };
}

static struct Clairvoyant_vTable {
    Player_vTable super;
} clairvoyant_vTable = {
    .super = {
        .show_summary = Clairvoyant_show_summary,
        .output_properties = Default_output_properties,
        .special_ability = Clairvoyant_special_ability,
        .delete = Default_dtor
    }
};


/*
* Public
*/
Clairvoyant* Clairvoyant_ctor(const int player_id) {
    Clairvoyant* clairvoyant = malloc(sizeof(Clairvoyant));
    if (!clairvoyant) exit(1);

    Player_init(&clairvoyant->super, (Player_vTable*)&clairvoyant_vTable, player_id, "CLAIRVOYANT");

    return clairvoyant;
}