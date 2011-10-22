#pragma once

#include <vector>

#define KEY_STATE_DOWN		0x1
#define KEY_STATE_CHANGED	0x2
#define KEY_COUNT			255

typedef unsigned __int8 Key;
typedef unsigned __int8 State;

class KeyboardHandler
{
public:
	KeyboardHandler();
	virtual ~KeyboardHandler();

	// Add a key to be handled.
	void AddKey( Key keyCode );

	// Updates the state of all keys.
	void UpdateKeys();

	// Returns true if the key is down.
	bool IsKeyPressed( Key keyCode );

	// Returns true if the key state changed.
	bool HasChangedState( Key keyCode );

	// Returns true when the key was released and is pressed.
	bool IsKeyClicked( Key keyCode );

	// Returns true when the key was pressed and is not pressed.
	bool IsKeyReleased( Key keyCode );

private:
	// Stores key states.
	State keyStates_[ KEY_COUNT ];
	std::vector<Key> keys_;
};