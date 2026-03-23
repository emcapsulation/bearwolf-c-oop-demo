#include "activist.h"
#include "../player_protected.h"
#include "../../util/util.h"

#include <stdio.h>
#include <stdlib.h>


/*
* Private
*/
typedef struct Activist_private {
    int last_banned;
    int voted_twice;
} Activist_private;

static Activist_private* Activist_private_ctor()
{
    Activist_private* private = malloc(sizeof(Activist_private));
    if (!private) exit(EXIT_FAILURE);

    private->last_banned = -1;
    private->voted_twice = 0;
    return private;
}


/*
* vTable Methods
*/
static void Activist_show_properties(Player* self)
{
    Default_show_summary(self);
    printf("\nYou can select one player to stop them voting in the day.\n");
}

static Event Activist_special_ability(Player* self, Player* target)
{
    if (!Player_is_alive(target))
    {
        printf("You cannot ban Player %d because they have died.", target->player_id);
        return DEFAULT_EVENT;
    }
    else if (((Activist*)self)->private->last_banned == target->player_id)
    {
        printf("You cannot stop the same person voting two nights in a row.");
        return DEFAULT_EVENT;
    }

    printf("Player %d cannot vote tomorrow.\n", target->player_id);
    Player_ban_vote(target);
    ((Activist*)self)->private->last_banned = target->player_id;

    return (Event){ .player_id = target->player_id, .action = BAN_VOTE };
}

static void Activist_dtor(Player* self)
{
    Activist* activist = (Activist*)self;
    free(activist->private);
    Default_dtor(&activist->super);
}

static const Player_vTable activist_vTable = {    
    .show_summary = Activist_show_properties,
    .output_properties = Default_output_properties,
    .gets_bitten = Default_gets_bitten,
    .special_ability = Activist_special_ability,
    .delete = Activist_dtor
};


/*
* Public
*/
Activist* Activist_ctor(const int player_id) {
    Activist* activist = malloc(sizeof(Activist));
    if (!activist) exit(EXIT_FAILURE);

    activist->private = Activist_private_ctor();
    Player_init(&activist->super, &activist_vTable, player_id, ACTIVIST);

    return activist;
}

int Activist_attempt_second_vote(Activist* self)
{
    if (self->private->voted_twice)
    {
        self->private->voted_twice = 0;
        return 0;
    }        

    printf("\nThe activist has a 1 in 2 chance of getting a second vote.\n");
    printf("PLAYER %d: Press ENTER to try and get a second vote.\n", self->super.player_id);
    Util_press_enter_to_continue();

    if (rand() % 2 == 0)
    {
        printf("SUCCESS - You can vote again.\n");
        self->private->voted_twice = 1;
        return 1;
    }
    else
    {
        printf("FAILURE - You only get one vote this round.\n");
        return 0;
    }
}