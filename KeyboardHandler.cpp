#include "KeyboardHandler.h"

KeyboardHandler::KeyboardHandler()
{
	// Keyboard handler created.
}

KeyboardHandler::~KeyboardHandler()
{
	// Keyboard handler destroyed.
}

void KeyboardHandler::listenKey( key keyCode )
{
	keys_.push_back( keyCode );
	keyStates_[ keyCode ] = 0;
}

void KeyboardHandler::updateKeys()
{
	int length = keys_.size();
	for (int i = 0; i < length; i++) {
		key currentKey = keys_.at( i );
		state currentState = keyStates_[ currentKey ];
		
		// Set it to down and unset if it's not pressed.
		keyStates_[ currentKey ] |= KEY_STATE_DOWN;
		if ((GetAsyncKeyState( currentKey ) & 0x8000) == 0) {
			keyStates_[ currentKey ] -= KEY_STATE_DOWN;
		}

		// Set flag to state change and unset if same.
		state newState = keyStates_[ currentKey ] | KEY_STATE_CHANGED;
		if (keyStates_[ currentKey ] == currentState ) {
			newState -= KEY_STATE_CHANGED;
		}
		keyStates_[ currentKey ] = newState;
	}
}

bool KeyboardHandler::changedState( key keyCode )
{
	return (keyStates_[ keyCode ] & KEY_STATE_CHANGED) != 0;
}

bool KeyboardHandler::isPressed( key keyCode )
{
	return (keyStates_[ keyCode ] & KEY_STATE_DOWN) != 0;
}

bool KeyboardHandler::isClicked( key keyCode )
{
	return isPressed( keyCode ) && changedState( keyCode );
}

bool KeyboardHandler::isReleased( key keyCode )
{
	return !isPressed( keyCode ) && changedState( keyCode );
}