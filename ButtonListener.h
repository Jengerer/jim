#pragma once

#include "Button.h"

class Button;
class ButtonListener
{
public:
	virtual void buttonPressed( Button* button ) = 0;
	virtual void buttonReleased( Button* button ) = 0;
};
