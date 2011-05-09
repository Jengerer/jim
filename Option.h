#pragma once

#include "Button.h"
#include "Texture.h"

class Option: public Button
{

public:

	Option( const string& caption );
	virtual ~Option( void );

	// Draw function.
	virtual void OnDraw( DirectX *directX );
	virtual void Pack( void );

};