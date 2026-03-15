#include "bear.h"
#include "../player_protected.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
* vTable Methods
*/
static void Bear_show_summary(Player* player)
{
    Default_show_summary(player);
    printf("\nYou can select one player to bite them.\n");
}

static void Bear_output_properties(Player* self, Player* player)
{
    if (!Player_is_alive(player))
        return;

    Default_output_properties(self, player);
    if (strcmp(player->role, "BEAR") == 0)
        printf("[bear]  ");
    else if (player->protected->is_bitten)
        printf("[bitten]  ");
}

static Event Bear_special_ability(Player* self, Player* target)
{
    if (strcmp(target->role, "BEAR") == 0)
    {
        printf("You cannot bite another bear.");
        return (Event){ .player_id = -1, .action = NO_ACTION };
    }

    printf("Player %d has been bitten.\n", target->player_id);
    target->protected->is_bitten = 1;
    
    return (Event){ .player_id = target->player_id, .action = BITE };
}

static struct Bear_vTable {
    Player_vTable super;
} bear_vTable = {
    .super = {
        .show_summary = Bear_show_summary,
        .output_properties = Bear_output_properties,
        .special_ability = Bear_special_ability,
        .delete = Default_dtor
    }
};


/*
* Public
*/
Bear* Bear_ctor(const int player_id) {
    Bear* bear = malloc(sizeof(Bear));
    if (!bear) exit(1);

    Player_init(&bear->super, (Player_vTable*)&bear_vTable, player_id, "BEAR");

    return bear;
}