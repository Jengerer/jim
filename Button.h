#pragma once

#include "Drawable.h"

class Button: public Drawable
{
public:
	Button( const string& caption, float x = 0.0f, float y = 0.0f );
	
	// Drawable functions.
	void draw( DirectX* directX );
	virtual void onMouseEvent( MouseListener* mouseListener, EMouseEvent mEvent );

	// Class-wide texture.
	virtual const Texture* getTexture() const;
	static Texture* texture;

private:
	bool isActive_;
	string caption_;
};