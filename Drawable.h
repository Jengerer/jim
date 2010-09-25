#pragma once

#include "DirectX.h"

class Drawable
{
public:
	Drawable( float x = 0.0f, float y = 0.0f );

	// Pure virtual; override me.
	virtual void draw( DirectX* directX ) = 0;

	// Position getter/setter.
	void setPosition( float x, float y );

	// For input handling.
	virtual void onMouseEvent( MouseListener* mouseListener, EMouseEvent mEvent ) = 0;
	bool mouseTouching( MouseListener* mouseListener ) const;

	// Virtual size getters; return texture size by default.
	virtual int getWidth() const;
	virtual int getHeight() const;

	// Drawing parameters.
	float	x, y;
	int		alpha;

	// Texture getter.
	virtual const Texture*	getTexture() const = 0;
};