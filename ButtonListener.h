#pragma once

#include "Button.h"

class Button;
class ButtonListener
{
public:
	virtual void onButtonClick( Button* button ) = 0;
	virtual void onButtonRelease( Button* button ) = 0;
};
