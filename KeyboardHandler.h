#pragma once

#include <vector>

#define KEY_STATE_DOWN		0x1
#define KEY_STATE_CHANGED	0x2
#define KEY_COUNT			255

typedef unsigned __int8 key;
typedef unsigned __int8 state;

class KeyboardHandler
{
public:
	KeyboardHandler();
	virtual ~KeyboardHandler();

	// Add a key to be handled.
	void AddKey( key keyCode );

	// Updates the state of all keys.
	void UpdateKeys();

	// Returns true if the key is down.
	bool IsKeyPressed( key keyCode );

	// Returns true if the key state changed.
	bool HasChangedState( key keyCode );

	// Returns true when the key was released and is pressed.
	bool IsKeyClicked( key keyCode );

	// Returns true when the key was pressed and is not pressed.
	bool IsKeyReleased( key keyCode );

private:
	// Stores key states.
	state keyStates_[ KEY_COUNT ];
	std::vector<key> keys_;
};