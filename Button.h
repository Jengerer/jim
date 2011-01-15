#pragma once

#include <vector>

#include "Panel.h"
#include "ButtonListener.h"
#include "Font.h"

using namespace std;

#define BUTTON_RADIUS	5
#define BUTTON_PADDING	20
#define BUTTON_COLOUR	D3DCOLOR_XRGB( 247, 231, 198 )

class ButtonListener;
class Button: public Panel
{
public:
	Button( const string& caption, 
		float x = 0.0f, float y = 0.0f, EAlignment align = ALIGN_TOP_LEFT );
	virtual ~Button();
	
	// Drawable functions.
	void			draw( DirectX* directX );

	// Class-wide texture.
	static Font*	font;

private:
	bool			isActive_;
	string			caption_;
};