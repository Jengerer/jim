#ifndef IKEYBOARD_HANDLER_H
#define IKEYBOARD_HANDLER_H

#include "keyboard.h"

class IKeyboardHandler
{

public:

	virtual bool on_key_pressed( Key key ) = 0;
	virtual bool on_key_released( Key key ) = 0;

};

#endif // IKEYBOARD_HANDLER_H