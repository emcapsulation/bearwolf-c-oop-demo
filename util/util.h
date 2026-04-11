#ifndef UTIL_H
#define UTIL_H

#include "../player/player.h"

extern const char* ASCII_ART[];

void Util_press_enter_to_continue();
int Util_get_valid_int_input(const int min_input, const int max_input, const char* prompt);
void Util_clear_screen();
void Util_shuffle_roles(Role* ints, const int num_players);

#endif
