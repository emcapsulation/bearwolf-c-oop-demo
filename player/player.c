#include "player.h"
#include "player_protected.h"
#include "impl/activist.h"
#include "impl/bear.h"
#include "impl/clairvoyant.h"
#include "impl/healer.h"
#include "impl/townsperson.h"
#include "../game/game.h"
#include "../util/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
* Private
*/
static Player_protected* Player_protected_ctor() 
{
    Player_protected* protected = malloc(sizeof(Player_protected));
    if (!protected) exit(EXIT_FAILURE);  

    protected->is_alive = 1;
    protected->can_vote = 1;
    protected->is_bitten = 0;

    return protected;
}

const Event DEFAULT_EVENT = {
    .player_id = -1,
    .action = NO_ACTION
};


/*
* Protected
*/
void super(Player* self, const Player_vTable* vTable, const int player_id, const Role role)
{
    self->vTable = vTable;
    self->player_id = player_id;
    self->role = role;
    self->protected = Player_protected_ctor();
}


/*
* Default vTable Methods
*/
void Default_show_summary(const Player* self)
{
    printf("\nRole: %s\n", Player_role_to_string(self->role));
    printf("%s", ASCII_ART[self->role]);
}

void Default_output_properties(const Player* self, const Player* player)
{
    if (!Player_is_alive(player))
        return;

    printf("*  Player %d  ", player->player_id);

    if (self->player_id == player->player_id)
        printf("[you]  ");
}

Event Default_gets_bitten(Player* self)
{
    if (!self->protected->is_alive)
    {
        printf("PLAYER %d cannot be bitten because they have died.", self->player_id);
        return DEFAULT_EVENT;
    }

    printf("PLAYER %d has been bitten.\n", self->player_id);
    self->protected->is_bitten = 1;
    return (Event) { .player_id = self->player_id, .action = BITE };
}

Event Default_special_ability(Player* self, Player* target)
{
    return (Event){ .player_id = -1, .action = NO_ACTION };
}


/*
* Public
*/
Player *Player_factory(const int player_id, const Role role)
{
    if (role == BEAR)
        return (Player*)Bear_ctor(player_id);
    else if (role == ACTIVIST)
        return (Player*)Activist_ctor(player_id);
    else if (role == CLAIRVOYANT)
        return (Player*)Clairvoyant_ctor(player_id);
    else if (role == HEALER)
        return (Player*)Healer_ctor(player_id);
    else if (role == TOWNSPERSON)
        return (Player*)Townsperson_ctor(player_id);
    else
        printf("\nInvalid role.\n");
    return NULL;
}

void Player_reset(Player* self)
{
    self->protected->can_vote = 1;
    self->protected->is_bitten = 0;
}

int Player_is_alive(const Player* self)
{
    return self->protected->is_alive;
}

void Player_eliminate(Player* self)
{
    self->protected->is_alive = 0;
}

int Player_can_vote(const Player* self)
{
    return self->protected->can_vote;
}

void Player_ban_vote(Player* self)
{
    self->protected->can_vote = 0;
}

int Player_is_bitten(const Player* self)
{
    return self->protected->is_bitten;
}

static const char* ROLE_NAMES[ROLE_COUNT] = {
    "BEAR",
    "ACTIVIST",
    "CLAIRVOYANT",
    "HEALER",
    "TOWNSPERSON"
};

const char* Player_role_to_string(Role role)
{
    return ROLE_NAMES[role];
}