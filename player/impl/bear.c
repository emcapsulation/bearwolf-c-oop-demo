#include "bear.h"
#include "../player_protected.h"

#include <stdio.h>
#include <stdlib.h>


/*
* vTable Methods
*/
static void Bear_show_summary(const Player* self)
{
    Default_show_summary(self);
    printf("\nYou can select one player to bite them.\n");
}

static Event Bear_special_ability(Player* self, Player* target)
{
    if (Player_gets_bitten(target))
    {
        printf("PLAYER %d has been bitten.\n", target->player_id);
        return (Event) { .target_player_id = target->player_id, .action = BITE };
    }
    return DEFAULT_EVENT;
}

static void Bear_dtor(Player* self)
{
    Bear* bear = (Bear*)self;
    Player_destroy(&bear->super);
    free(bear);
}

static const Player_vTable bear_vTable = {
    .show_summary = Bear_show_summary,
    .special_ability = Bear_special_ability,    
    .destroy = Bear_dtor
};


/*
* Public
*/
Bear* Bear_ctor(const int player_id) 
{
    Bear* bear = malloc(sizeof(Bear));
    if (!bear) exit(EXIT_FAILURE);

    super((Player*)bear, &bear_vTable, player_id, BEAR);

    return bear;
}


void Bear_output_player(const Player* output_player)
{
    if (output_player->role == BEAR)
        printf("[bear]  ");
    else if (Player_is_bitten(output_player))
        printf("[bitten]  ");
}