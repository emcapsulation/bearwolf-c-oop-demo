#include "player.h"
#include "player_protected.h"
#include "impl/activist.h"
#include "impl/bear.h"
#include "impl/clairvoyant.h"
#include "impl/healer.h"
#include "impl/townsperson.h"
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

Event Default_special_ability(Player* self, Player* target)
{
    return DEFAULT_EVENT;
}


/*
* Public
*/
Player* Player_factory(const int player_id, const Role role)
{
    switch (role) {
    case BEAR: return (Player*)Bear_ctor(player_id);
    case ACTIVIST: return (Player*)Activist_ctor(player_id);
    case CLAIRVOYANT: return (Player*)Clairvoyant_ctor(player_id);
    case HEALER: return (Player*)Healer_ctor(player_id);
    case TOWNSPERSON: return (Player*)Townsperson_ctor(player_id);
    default: return NULL;
    }
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

int Player_ban_vote(Player* self)
{
    if (!self->protected->is_alive)
    {
        printf("PLAYER %d cannot be banned because they have died.", self->player_id);
        return 0;
    }

    self->protected->can_vote = 0;
    return 1;
}

int Player_is_bitten(const Player* self)
{
    return self->protected->is_bitten;
}

int Player_gets_bitten(Player* self)
{
    if (!self->protected->is_alive)
    {
        printf("PLAYER %d cannot be bitten because they have died.", self->player_id);
        return 0;
    }
    else if (self->role == BEAR)
    {
        printf("PLAYER %d cannot be bitten because they are a bear.", self->player_id);
        return 0;
    }

    self->protected->is_bitten = 1;
    return 1;
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

const Event DEFAULT_EVENT = {
    .target_player_id = -1,
    .action = NO_ACTION
};