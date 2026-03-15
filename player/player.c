#include "player.h"
#include "player_protected.h"
#include "impl/activist.h"
#include "impl/bear.h"
#include "impl/clairvoyant.h"
#include "impl/healer.h"
#include "impl/townsperson.h"
#include "../game/game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
* Private
*/
static Player_protected* Player_protected_ctor() 
{
    Player_protected* protected = malloc(sizeof(Player_protected));
    if (!protected) exit(1);  

    protected->is_alive = 1;

    return protected;
}


/*
* Protected
*/
void Player_init(Player* self, Player_vTable* vTable, const int player_id, const Role role)
{
    self->vTable = vTable;
    self->player_id = player_id;
    strncpy(self->role, role, sizeof(self->role)-1);
    self->protected = Player_protected_ctor();
    Player_reset(self);
}


/*
* Default vTable Methods
*/
void Default_show_summary(Player* self)
{
    printf("\nRole: %s", self->role);
}

void Default_output_properties(Player* self, Player* player)
{
    if (!Player_is_alive(player))
        return;

    printf("*  Player %d  ", player->player_id);

    if (self->player_id == player->player_id)
        printf("[you]  ");
}

Event Default_special_ability(Player* self, Player* target)
{
    return (Event){ .player_id = -1, .action = NO_ACTION };
}

void Default_dtor(Player* self)
{
    free(self->protected);
    free(self);
}


/*
* Public
*/
Player *Player_factory(const int player_id, const Role role)
{
    if (strcmp(role, "BEAR") == 0)
        return (Player *)Bear_ctor(player_id);
    else if (strcmp(role, "ACTIVIST") == 0)
        return (Player*)Activist_ctor(player_id);
    else if (strcmp(role, "CLAIRVOYANT") == 0)
        return (Player*)Clairvoyant_ctor(player_id);
    else if (strcmp(role, "HEALER") == 0)
        return (Player*)Healer_ctor(player_id);
    else if (strcmp(role, "TOWNSPERSON") == 0)
        return (Player*)Townsperson_ctor(player_id);
    else
        printf("\nInvalid role.\n");
    return NULL;
}

void Player_reset(Player* player)
{
    player->protected->can_vote = 1;
    player->protected->is_bitten = 0;
}

int Player_is_alive(const Player* player)
{
    return player->protected->is_alive;
}

void Player_eliminate(Player* player)
{
    player->protected->is_alive = 0;
}

int Player_can_vote(const Player* player)
{
    return player->protected->can_vote;
}


/*
* vTable Methods
*/
void Player_show_summary(Player* self)
{
    self->vTable->show_summary(self);
}

void Player_output_properties(Player* self, Player* player)
{
    self->vTable->output_properties(self, player);
}

Event Player_special_ability(Player* self, Player* target)
{
    return self->vTable->special_ability(self, target);
}

void delete(Player* self)
{
    self->vTable->delete(self);
}