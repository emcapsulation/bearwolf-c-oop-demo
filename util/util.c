#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void Util_press_enter_to_continue()
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}


int Util_get_valid_int_input(const int min_input, const int max_input, const char *prompt)
{
	int input;
	int input_status;
	int c;
	do
	{
		printf("%s", prompt);
		input_status = scanf("%d", &input);
		while ((c = getchar()) != '\n' && c != EOF);

	} while (input_status != 1 || 
		input < min_input || 
		input > max_input);

	return input;
}


void Util_clear_screen()
{
	printf("\033[H\033[J");
}


/*
* Fisher-Yates Shuffle Algorithm
* Given an array, it generates a random permuation.
*/
void Util_shuffle_ints(int* ints, const int num_players)
{
	for (int i = num_players - 1; i > 0; i--)
	{
		int j = rand() % (i + 1);
		int tmp = ints[i];
		ints[i] = ints[j];
		ints[j] = tmp;
	}
}