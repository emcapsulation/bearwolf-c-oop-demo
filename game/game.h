#ifndef GAME_H
#define GAME_H

#define NUM_BEARS 2

typedef struct Game_context {
	const int num_players;
	int player_bitten_ids[NUM_BEARS];
	int player_healed_id;
	int player_banned_id;
} Game_context;
typedef struct Game_internals Game_internals;

typedef struct Game {
	Game_context *context;
	Game_internals* internals;
} Game;

Game *Game_ctor(const int num_players);
void Game_loop(Game* game);
void Game_dtor(Game* game);

#endif