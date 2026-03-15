#ifndef GAME_H
#define GAME_H

#define NUM_BEARS 2

/*
* Encapsulation: Game is an opaque pointer.
*/
typedef struct Game Game;

typedef enum Action {
	NO_ACTION,
	BITE,
	BAN_VOTE,
	SEE,
	HEAL
} Action;

typedef struct Event {
	int player_id;
	Action action;
} Event;

Game *Game_ctor(const int num_players);
void Game_loop(Game* game);
void Game_dtor(Game* game);

#endif