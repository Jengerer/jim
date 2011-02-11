#pragma once

#include "Button.h"

#define MENU_BUTTON_PADDING_X	15
#define MENU_BUTTON_PADDING_Y	10
#define MENU_BUTTON_RADIUS		3

class MenuOption: public Button
{
public:
	MenuOption( const string& caption );
	virtual ~MenuOption();

	// Draw function.
	virtual void draw( DirectX *directX );
};