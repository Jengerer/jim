#include "keyboard.h"

/*
 * Keyboard contructor.
 */
Keyboard::Keyboard()
{
	clear_states();
}

/*
 * Returns true if a key is pressed.
 */
bool Keyboard::is_key_pressed( int key_code ) const
{
	return states_[key_code];
}

/*
 * Updates a key's state.
 */
void Keyboard::set_key_state( int key, bool is_pressed )
{
	states_[key] = is_pressed;
}

/*
 * Resets all key states.
 */
void Keyboard::clear_states()
{
	for (size_t i = 0; i < KEY_COUNT; ++i) {
		states_[i] = false;
	}
}