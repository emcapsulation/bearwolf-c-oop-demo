#include "game.h"
#include "../player/player.h"
#include "../player/impl/bear.h"
#include "../player/impl/activist.h"
#include "../player/impl/healer.h"
#include "../util/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
* Private
*/
struct Game {
	int num_players;
	int num_bears_left;
	int num_townspeople_left;
	int is_night;

	int player_bitten_ids[NUM_BEARS];
	int bites_this_round;
	int player_healed_id;
	Player** players;	
};


static int Game_get_winner(const Game *self) 
{
	if (self->num_bears_left >= self->num_townspeople_left)
		return 1;	
	if (self->num_bears_left == 0)
		return 2;

	return 0;
}


static void Game_handle_event(Game* self, Event event)
{
	if (event.action == BITE)
		self->player_bitten_ids[self->bites_this_round++] = event.player_id;
	else if (event.action == HEAL)
		self->player_healed_id = event.player_id;
}


static void Game_do_player_special_ability(Game* self, Player *player)
{
	int input;
	Event event;
	do
	{
		input = Util_get_valid_int_input(
			1, self->num_players, 
			"\n\nEnter the target Player ID: ");

		event = player->vTable->special_ability(player, self->players[input - 1]);
	} while (event.player_id == -1);

	Game_handle_event(self, event);
}


static void Game_output_player(Player* cur_player, Player* output_player)
{
	if (!Player_is_alive(output_player))
		return;

	printf("*  Player %d  ", output_player->player_id);

	if (output_player->player_id == cur_player->player_id)
		printf("[you]  ");

	if (cur_player->role == BEAR)
		Bear_output_player(output_player);
}


static void Game_do_player_round(Game *self, Player *player)
{	
	player->vTable->show_summary(player);

	if (player->role != TOWNSPERSON)
	{
		printf("\nPLAYERS:\n");
		for (int pid = 0; pid < self->num_players; pid++)
			Game_output_player(player, self->players[pid]);

		Game_do_player_special_ability(self, player);
	}

	printf("\nPress ENTER to clear the screen.\n");
	Util_press_enter_to_continue();
	Util_clear_screen();
}


static void Game_do_night_round(Game* self)
{
	Util_clear_screen();
	printf("\n\n******** NIGHT ********");
	printf("\nIt is night time.");
	
	for (int pid = 0; pid < self->num_players; pid++)
	{
		Player* cur_player = self->players[pid];

		if (!Player_is_alive(cur_player))
			continue;

		printf("\n\nPLAYER %d\nPress ENTER when ready. ", cur_player->player_id);
		Util_press_enter_to_continue();
		Game_do_player_round(self, cur_player);
	}
}


static void Game_eliminate_player(Game* self, Player* eliminated)
{
	if (eliminated->role == HEALER && Healer_attempt_self_heal((Healer *)eliminated))
		return;

	if (eliminated->role == BEAR)
		self->num_bears_left--;
	else
		self->num_townspeople_left--;

	Player_eliminate(eliminated);
}


static void Game_reveal_night_results(Game* self)
{
	printf("\n\n******** DAY ********\n");
	printf("It is daytime.\n");

	int bitten_index = self->num_bears_left == 2 ? rand() % NUM_BEARS : 0;
	int bitten_id = self->player_bitten_ids[bitten_index];
	if (bitten_id == self->player_healed_id)
	{
		printf("\nNo one died last night.\n");
	}
	else
	{
		printf("\nPlayer %d died last night.\n", bitten_id);

		Player* bitten_player = self->players[bitten_id-1];
		printf("Player %d was a %s.\n", bitten_player->player_id, Player_role_to_string(bitten_player->role));

		Game_eliminate_player(self, bitten_player);
	}
}


static void Game_init_round_properties(Game* self)
{
	for (int i = 0; i < NUM_BEARS; i++)
		self->player_bitten_ids[i] = -1;
	self->bites_this_round = 0;
	self->player_healed_id = -1;
}


static void Game_reset(Game* self)
{
	Game_init_round_properties(self);
	for (int i = 0; i < self->num_players; i++)
	{
		Player_reset(self->players[i]);
	}
}


static int Game_show_vote_prompt(Game *self, Player* cur_player, int *votes)
{
	if (!Player_is_alive(cur_player))
		return 0;

	printf("\n\nPLAYER %d\nPress ENTER when ready.\n", cur_player->player_id);
	Util_press_enter_to_continue();

	if (!Player_can_vote(cur_player))
	{
		printf("The activist has banned you from voting!\n");
		return 0;
	}

	for (int i = 0; i < self->num_players; i++)
		if (Player_is_alive(self->players[i]))
			printf("Player %d [%d]  ", i + 1, votes[i]);
	printf("\n");

	return 1;
}


static void Game_do_vote_round(Game* self)
{
	int* votes = calloc(self->num_players, sizeof(int));
	int max_votes[2] = { -1, -1 };

	printf("\n\n******** VOTING ********\n");
	printf("Time to vote out who you think is the bear.\n");

	for (int pid = 0; pid < self->num_players; pid++)
	{
		Player* cur_player = self->players[pid];
		if (!Game_show_vote_prompt(self, cur_player, votes))
			continue;		

		int input = -1;		
		do 
		{
			input = Util_get_valid_int_input(
				1, self->num_players,
				"\nEnter the Player ID to vote for them: "
			) - 1;
		} while (!Player_is_alive(self->players[input]));
		votes[input]++;

		if (votes[input] > max_votes[0])
		{
			max_votes[0] = votes[input];
			max_votes[1] = input;
		}

		if (cur_player->role == ACTIVIST && Activist_attempt_second_vote((Activist *)cur_player))
			pid--;
	}

	Player* voted_player = self->players[max_votes[1]];

	printf("\nPlayer %d got the most votes and is out.\n", voted_player->player_id);
	printf("Role: %s\n", Player_role_to_string(voted_player->role));

	Game_eliminate_player(self, voted_player);

	free(votes);
	Game_reset(self);
}


static Player** Game_create_player_list(Player** players, const int num_players)
{
	Role role_list[5] = {BEAR, BEAR, ACTIVIST, CLAIRVOYANT, HEALER};
	Role* all_roles = malloc(sizeof(Role) * num_players);
	if (!all_roles) exit(EXIT_FAILURE);

	int rid = 0;
	for (int pid = 0; pid < num_players; pid++)
	{
		all_roles[pid] = rid < 5 ? role_list[rid] : TOWNSPERSON;
		rid++;
	}

	Util_shuffle_ints((int *)all_roles, num_players);

	for (int pid = 0; pid < num_players; pid++)
		players[pid] = Player_factory(pid + 1, all_roles[pid]);

	free(all_roles);
	return players;
}


static Player** Game_init_game_players(const int num_players)
{
	Player** players = malloc(num_players * sizeof(Player*));
	if (!players) exit(EXIT_FAILURE);

	players = Game_create_player_list(players, num_players);

	return players;
}


/*
* Public
*/
Game* Game_ctor(const int num_players)
{
	Game* game = malloc(sizeof(Game));
	if (!game) exit(EXIT_FAILURE);	

	game->num_players = num_players;
	game->is_night = 1;
	game->num_bears_left = NUM_BEARS;
	game->num_townspeople_left = num_players - NUM_BEARS;
	Game_init_round_properties(game);
	game->players = Game_init_game_players(num_players);	

	return game;
}


void Game_loop(Game* self) 
{
	while (!Game_get_winner(self)) 
	{
		printf("\nPress ENTER to continue.\n");
		Util_press_enter_to_continue();

		if (self->is_night)
		{
			Game_do_night_round(self);
			Game_reveal_night_results(self);
		}			
		else
		{
			Game_do_vote_round(self);
		}

		self->is_night = !self->is_night;
	}

	printf("\n\n%s WIN\n\n", Game_get_winner(self) == 1 ? "BEARS" : "TOWNSPEOPLE");
}


void Game_dtor(Game* self) 
{
	for (int i = 0; i < self->num_players; i++)
	{
		Player* player = self->players[i];
		player->vTable->destroy(player);
	}
	free(self->players);
	free(self);
}