#ifndef UTIL_H
#define UTIL_H

typedef char Role[16];

void Util_press_enter_to_continue();
int Util_get_valid_int_input(const int min_input, const int max_input, const char* prompt);
void Util_shuffle_roles(Role* roles, const int num_players);

#endif
