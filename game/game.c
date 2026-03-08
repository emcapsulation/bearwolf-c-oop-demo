#include "game.h"
#include "../player/player.h"
#include "../util/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * Private
 */
typedef struct Game_internals
{
	int num_players;
	int num_bears;
	int num_townspeople;
	int is_night;
	Player** players;
} Game_internals;


int Game_get_winner(const Game *self) 
{
	if (self->internals->num_bears >= self->internals->num_townspeople)
		return 0;
	
	if (self->internals->num_bears == 0)
		return 1;

	return -1;
}


void Game_player_select_special(Game* self, Player *cur_player)
{
	int input;
	do
	{
		input = Util_get_valid_int_input(
			1, self->internals->num_players, 
			"\n\nEnter the target Player ID: ");

	} while (!(Player_special_ability(cur_player, 
		self->internals->players[input-1], self->context)));
}


void Game_display_players(Game *self, Player *cur_player)
{
	if (strcmp(cur_player->role, "TOWNSPERSON") == 0)
		return;

	printf("\nPLAYERS:\n");
	for (int pid = 0; pid < self->internals->num_players; pid++)
		Player_output_properties(
			cur_player,
			self->internals->players[pid]
		);

	Game_player_select_special(self, cur_player);
}


void Game_do_night_round(Game* self)
{
	printf("\n\n******** NIGHT ********");
	printf("\nIt is night time.");
	
	for (int pid = 0; pid < self->internals->num_players; pid++)
	{
		Player* cur_player = self->internals->players[pid];

		if (!Player_is_alive(cur_player))
			continue;		

		printf("\n\nPLAYER %d\nPress enter when ready. ", cur_player->player_id);
		Util_press_enter_to_continue();
		
		Player_show_summary(cur_player);
		Game_display_players(self, cur_player);
	}
}


void Game_eliminate_player(Game* self, Player* eliminated)
{
	if (strcmp(eliminated->role, "BEAR") == 0)
		self->internals->num_bears--;
	else
		self->internals->num_townspeople--;

	Player_eliminate(eliminated);
}


void Game_reveal_night_results(Game* self)
{
	printf("\n\n******** DAY ********\n");
	printf("It is daytime.\n");

	int bitten_index = self->internals->num_bears == 2 ? rand() % NUM_BEARS : 0;
	int bitten_id = self->context->player_bitten_ids[bitten_index];
	if (bitten_id == self->context->player_healed_id)
	{
		printf("\nNo one died last night.\n");
	}
	else
	{
		printf("\nPlayer %d died last night.\n", bitten_id);

		Player* bitten_player = self->internals->players[bitten_id-1];
		printf("Player %d was a %s.\n", bitten_player->player_id, bitten_player->role);

		Game_eliminate_player(self, bitten_player);
	}

	printf("\nPress enter when you are ready to vote.\n");
	Util_press_enter_to_continue();
}


void Game_reset_context(Game_context* context)
{
	context->player_bitten_ids[0] = -1;
	context->player_bitten_ids[1] = -1;
	context->player_banned_id = -1;
	context->player_healed_id = -1;
}


void Game_reset(Game* self)
{
	Game_reset_context(self->context);
	for (int i = 0; i < self->internals->num_players; i++)
	{
		Player_reset(self->internals->players[i]);
	}
}


int Game_show_vote_prompt(Game *self, Player* cur_player, int *votes)
{
	if (!Player_is_alive(cur_player))
		return 0;

	printf("\nPLAYER %d\nPress enter when ready.\n", cur_player->player_id);
	Util_press_enter_to_continue();

	if (!Player_can_vote(cur_player))
	{
		printf("The activist has banned you from voting!\n");
		return 0;
	}

	for (int i = 0; i < self->internals->num_players; i++)
		if (Player_is_alive(self->internals->players[i]))
			printf("Player %d [%d]  ", i + 1, votes[i]);
	printf("\n");

	return 1;
}


void Game_do_vote_round(Game* self)
{
	int* votes = calloc(self->internals->num_players, sizeof(int));
	int max_votes[2] = { -1, -1 };

	printf("\n\n******** VOTING ********\n");
	printf("Time to vote out who you think is the bear.\n");

	for (int pid = 0; pid < self->internals->num_players; pid++)
	{
		Player* cur_player = self->internals->players[pid];
		if (!Game_show_vote_prompt(self, cur_player, votes))
			continue;		

		int input = Util_get_valid_int_input(
			1, self->internals->num_players, 
			"\nEnter the Player ID to vote for them: "
		)-1;
		votes[input]++;

		if (votes[input] > max_votes[0])
		{
			max_votes[0] = votes[input];
			max_votes[1] = input;
		}
	}

	Player* voted_player = self->internals->players[max_votes[1]];

	printf("\nPlayer %d got the most votes and is out.\n", voted_player->player_id);
	printf("Role: %s\n", voted_player->role);

	Game_eliminate_player(self, voted_player);

	free(votes);
	Game_reset(self);
}


// CONSTRUCTOR LOGIC
Player** p_create_player_list(Player** players, const int num_players)
{
	char* role_list[5] = {"BEAR", "BEAR", "ACTIVIST", "CLAIRVOYANT", "HEALER"};
	Role* all_roles = malloc(sizeof(Role) * num_players);
	if (!all_roles) exit(1);

	int rid = 0;
	for (int pid = 0; pid < num_players; pid++)
	{
		strncpy(all_roles[pid], rid < 5 ? role_list[rid] : "TOWNSPERSON", sizeof(Role)-1);
		rid++;	
	}

	Util_shuffle_roles(all_roles, num_players);

	for (int pid = 0; pid < num_players; pid++)
		players[pid] = Player_factory(pid + 1, all_roles[pid]);

	free(all_roles);
	return players;
}


Player** Game_init_game_players(const int num_players)
{
	Player** players = malloc(num_players * sizeof(Player*));
	if (!players) exit(1);

	players = p_create_player_list(players, num_players);

	return players;
}


Game_context* Game_context_ctor()
{
	Game_context* context = malloc(sizeof(Game_context));
	if (!context) exit(1);

	Game_reset_context(context);

	return context;
}


Game_internals* Game_internals_ctor(const int num_players)
{
	Game_internals* internals = malloc(sizeof(Game_internals));
	if (!internals) exit(1);

	internals->num_players = num_players;
	internals->is_night = 1;
	internals->num_bears = NUM_BEARS;
	internals->num_townspeople = num_players - NUM_BEARS;
	internals->players = Game_init_game_players(num_players);

	return internals;
}


/**
 * Public
 */
Game* Game_ctor(const int num_players)
{
	Game* game = malloc(sizeof(Game));
	if (!game) exit(1);

	game->context = Game_context_ctor();
	game->internals = Game_internals_ctor(num_players);

	return game;
}


void Game_loop(Game* self) 
{
	while (Game_get_winner(self) == -1) 
	{
		if (self->internals->is_night)
		{
			Game_do_night_round(self);
			Game_reveal_night_results(self);
		}			
		else
		{
			Game_do_vote_round(self);
		}

		self->internals->is_night = !self->internals->is_night;
	}

	printf("\n\n%s WIN\n\n", Game_get_winner(self) == 0 ? "BEARS" : "TOWNSPEOPLE");
}


void Game_dtor(Game* self) 
{
	for (int i = 0; i < self->internals->num_players; i++)
	{
		delete(self->internals->players[i]);
	}
	free(self->internals->players);
	free(self->internals);
	free(self->context);
	free(self);
}