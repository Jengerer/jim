#pragma once

#include <vector>

#include "Drawable.h"
#include "ButtonListener.h"

using namespace std;

class ButtonListener;
class Button: public Drawable
{
public:
	Button( const string& caption, float x = 0.0f, float y = 0.0f );
	
	// Drawable functions.
	void			draw( DirectX* directX );
	virtual int		getWidth() const;
	virtual int		getHeight() const;

	// Class-wide texture.
	static Texture*	texture;

private:
	bool			isActive_;
	string			caption_;
};