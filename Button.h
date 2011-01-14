#pragma once

#include <vector>

#include "Panel.h"
#include "ButtonListener.h"
#include "Font.h"

using namespace std;

class ButtonListener;
class Button: public Panel
{
public:
	Button( const string& caption, 
		float x = 0.0f, float y = 0.0f );
	virtual ~Button();
	
	// Drawable functions.
	void			draw( DirectX* directX );
	virtual int		getWidth() const;
	virtual int		getHeight() const;

	// Class-wide texture.
	static Texture*	texture;
	static Font*	font;

private:
	bool			isActive_;
	string			caption_;
};