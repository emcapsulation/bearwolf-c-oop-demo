#include "healer.h"
#include "../player_protected.h"

#include <stdio.h>
#include <stdlib.h>


/*
* Private
*/
typedef struct Healer_private {
    int last_healed;
} Healer_private;

static Healer_private* Healer_private_ctor() 
{
    Healer_private* private = malloc(sizeof(Healer_private));
    if (!private) exit(EXIT_FAILURE);

    private->last_healed = -1;
    return private;
}


/*
* vTable Methods
*/
static void Healer_show_summary(Player* self)
{
    Default_show_summary(self);
    printf("\nYou can select one player to protect them from the bears.\n");
}

static Event Healer_special_ability(Player* self, Player* target)
{
    if (!Player_is_alive(target))
    {
        printf("You cannot heal Player %d because they have died.", target->player_id);
        return (Event) { .player_id = -1, .action = NO_ACTION };
    }
    else if (target->player_id == ((Healer*)self)->private->last_healed)
    {
        printf("You cannot heal the same player two nights in a row.\n");
        return (Event){ .player_id = -1, .action = NO_ACTION };
    }

    printf("Player %d is protected for the night.\n", target->player_id);
    ((Healer*)self)->private->last_healed = target->player_id;

    return (Event){ .player_id = target->player_id, .action = HEAL };
}

static void Healer_dtor(Player* self)
{
    Healer* healer = (Healer*)self;
    free(healer->private);
    Default_dtor(&healer->super);
}

static const Player_vTable healer_vTable = {
    .show_summary = Healer_show_summary,
    .output_properties = Default_output_properties,
    .gets_bitten = Default_gets_bitten,
    .special_ability = Healer_special_ability,
    .delete = Healer_dtor
};


/*
* Public
*/
Healer* Healer_ctor(const int player_id) {
    Healer* healer = malloc(sizeof(Healer));
    if (!healer) exit(EXIT_FAILURE);

    healer->private = Healer_private_ctor();
    Player_init(&healer->super, &healer_vTable, player_id, HEALER);

    return healer;
}

int Healer_attempt_self_heal(Healer* self)
{
    printf("\nThe healer has a 1 in 2 chance of healing themselves!\n");
    printf("PLAYER %d: Press ENTER to try and heal yourself.\n", self->super.player_id);
    Util_press_enter_to_continue();

    if (rand() % 2 == 0)
    {
        printf("SUCCESS - The healer lives.\n");
        return 1;
    }
    else 
    {
        printf("FAILURE - The healer is eliminated.\n");
        return 0;
    }
}