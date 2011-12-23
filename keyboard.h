#ifndef KEYBOARD_H
#define KEYBOARD_H

// Number of key codes.
const size_t KEY_COUNT = 255;

class Keyboard
{

public:

	Keyboard();
	
	// Keyboard state getters.
	bool is_key_pressed( int code ) const;
	
	// State updater.
	void set_key_state( int code, bool is_pressed );

	// Reset key states.
	void clear_states();

private:

	// Stores key states.
	bool states_[ KEY_COUNT ];

};

#endif // KEYBOARD_H