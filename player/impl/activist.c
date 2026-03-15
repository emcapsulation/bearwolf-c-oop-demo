#include "activist.h"
#include "../player_protected.h"

#include <stdio.h>
#include <stdlib.h>


/*
* Private
*/
typedef struct Activist_private {
    int last_banned;
} Activist_private;

static Activist_private* Activist_private_ctor()
{
    Activist_private* private = malloc(sizeof(Activist_private));
    if (!private) exit(EXIT_FAILURE);

    private->last_banned = -1;
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
        return (Event) { .player_id = -1, .action = NO_ACTION };
    }
    else if (((Activist*)self)->private->last_banned == target->player_id)
    {
        printf("You cannot stop the same person voting two nights in a row.");
        return (Event){ .player_id = -1, .action = NO_ACTION };
    }

    printf("Player %d cannot vote tomorrow.\n", target->player_id);
    target->protected->can_vote = 0;
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
