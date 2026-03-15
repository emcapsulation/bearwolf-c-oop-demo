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
    if (player->role == BEAR)
        printf("[bear]  ");
    else if (Player_is_bitten(player))
        printf("[bitten]  ");
}

static Event Bear_gets_bitten(Player* self)
{
    if (!self->protected->is_alive)
        printf("PLAYER %d cannot be bitten because they have died.", self->player_id);
    else
        printf("PLAYER %d cannot be bitten because they are a bear.", self->player_id);
    return DEFAULT_EVENT;
}

static Event Bear_special_ability(Player* self, Player* target)
{    
    return target->vTable->gets_bitten(target);
}

static const Player_vTable bear_vTable = {
    .show_summary = Bear_show_summary,
    .output_properties = Bear_output_properties,
    .gets_bitten = Bear_gets_bitten,
    .special_ability = Bear_special_ability,    
    .delete = Default_dtor
};


/*
* Public
*/
Bear* Bear_ctor(const int player_id) {
    Bear* bear = malloc(sizeof(Bear));
    if (!bear) exit(EXIT_FAILURE);

    Player_init(&bear->super, &bear_vTable, player_id, BEAR);

    return bear;
}