#include "game/game.h"
#include "util/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main() {
	srand(time(NULL));

	printf("\n\n\n******** Welcome to Bear-Wolf: The C-OOP Demo ********\n");
	printf("This minigame is designed to teach object-oriented programming\n" \
		"techniques in C.\n");
	printf("%s", ASCII_ART[0]);

	int num_players = Util_get_valid_int_input(
		6, 20, "\nEnter the number of players [6, 20]: ");
	
	Game* game = Game_ctor(num_players);
	Game_loop(game);
	Game_dtor(game);

	return 0;
}