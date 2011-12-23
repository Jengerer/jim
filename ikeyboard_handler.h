#ifndef IKEYBOARD_HANDLER_H
#define IKEYBOARD_HANDLER_H

class IKeyboardHandler
{

public:

	virtual bool on_key_pressed( int key ) = 0;
	virtual bool on_key_released( int key ) = 0;

};

#endif // IKEYBOARD_HANDLER_H