#include "clairvoyant.h"
#include "../player_protected.h"

#include <stdio.h>
#include <stdlib.h>


/*
* vTable Methods
*/
static void Clairvoyant_show_summary(const Player* self)
{
    Default_show_summary(self);
    printf("\nYou can select one player to see their role.\n");
}

static Event Clairvoyant_special_ability(Player* self, Player* target)
{
    printf("You see PLAYER %d's role: %s.\n", target->player_id, Player_role_to_string(target->role));
    return (Event) { .target_player_id = target->player_id, .action = SEE };
}

static void Clairvoyant_dtor(Player* self)
{
    Clairvoyant* clairvoyant = (Clairvoyant*)self;
    free(clairvoyant->super.protected);
    free(clairvoyant);
}

static const Player_vTable clairvoyant_vTable = {
    .show_summary = Clairvoyant_show_summary,
    .special_ability = Clairvoyant_special_ability,
    .destroy = Clairvoyant_dtor
};


/*
* Public
*/
Clairvoyant* Clairvoyant_ctor(const int player_id) 
{
    Clairvoyant* clairvoyant = malloc(sizeof(Clairvoyant));
    if (!clairvoyant) exit(EXIT_FAILURE);

    super((Player*)clairvoyant, &clairvoyant_vTable, player_id, CLAIRVOYANT);

    return clairvoyant;
}