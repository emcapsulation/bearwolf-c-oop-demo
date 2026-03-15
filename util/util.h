#ifndef UTIL_H
#define UTIL_H

typedef enum {
	BEAR,
	ACTIVIST,
	CLAIRVOYANT,
	HEALER,
	TOWNSPERSON,
	ROLE_COUNT
} Role;

void Util_press_enter_to_continue();
int Util_get_valid_int_input(const int min_input, const int max_input, const char* prompt);
void Util_shuffle_roles(Role* roles, const int num_players);
const char* Util_role_to_string(Role role);

#endif
