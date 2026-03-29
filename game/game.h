#ifndef GAME_H
#define GAME_H

#define NUM_BEARS 2

/*
* Encapsulation: Game is an opaque pointer.
*/
typedef struct Game Game;

Game *Game_ctor(const int num_players);
void Game_loop(Game* self);
void Game_dtor(Game* self);

#endif