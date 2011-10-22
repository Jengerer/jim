#include "keyboard.h"

#include <windows.h>

KeyboardHandler::KeyboardHandler()
{
	// Keyboard handler created.
}

KeyboardHandler::~KeyboardHandler()
{
	// Keyboard handler destroyed.
}

void KeyboardHandler::AddKey( Key keyCode )
{
	keys_.push_back( keyCode );
	keyStates_[ keyCode ] = 0;
}

void KeyboardHandler::UpdateKeys()
{
	int length = keys_.size();
	for (int i = 0; i < length; i++) {
		Key currentKey = keys_.at( i );
		State currentState = keyStates_[ currentKey ];
		
		// Set it to down and unset if it's not pressed.
		keyStates_[ currentKey ] |= KEY_STATE_DOWN;
		if ((GetAsyncKeyState( currentKey ) & 0x8000) == 0) {
			keyStates_[ currentKey ] -= KEY_STATE_DOWN;
		}

		// Set flag to state change and unset if same.
		State newState = keyStates_[ currentKey ] | KEY_STATE_CHANGED;
		if (keyStates_[ currentKey ] == currentState ) {
			newState -= KEY_STATE_CHANGED;
		}
		keyStates_[ currentKey ] = newState;
	}
}

bool KeyboardHandler::HasChangedState( Key keyCode )
{
	return (keyStates_[ keyCode ] & KEY_STATE_CHANGED) != 0;
}

bool KeyboardHandler::IsKeyPressed( Key keyCode )
{
	return (keyStates_[ keyCode ] & KEY_STATE_DOWN) != 0;
}

bool KeyboardHandler::IsKeyClicked( Key keyCode )
{
	return IsKeyPressed( keyCode ) && HasChangedState( keyCode );
}

bool KeyboardHandler::IsKeyReleased( Key keyCode )
{
	return !IsKeyPressed( keyCode ) && HasChangedState( keyCode );
}