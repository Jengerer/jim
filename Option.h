#pragma once

#include "Button.h"
#include "Texture.h"

#define MENU_BUTTON_PADDING_X	15
#define MENU_BUTTON_PADDING_Y	10
#define MENU_BUTTON_RADIUS		3

class Option: public Button
{
public:
	Option( const string& caption, Texture *texture = 0 );
	virtual ~Option();

	// Draw function.
	virtual void draw( DirectX *directX );
	virtual void pack();
	void setTexture( Texture *texture );
};