#pragma once

#include <vector>

#include "Mouse.h"
#include "Drawable.h"

using namespace std;

// Prototypes.
class Mouse;
class Component: public Drawable {
public:
	Component( float x = 0.0f, float y = 0.0f );
	virtual ~Component();

	// Mouse handling functions.
	virtual bool leftClicked( Mouse *mouse );
	virtual bool leftReleased( Mouse *mouse );
	virtual bool rightClicked( Mouse *mouse );
	virtual bool rightReleased( Mouse *mouse );
	virtual bool mouseMoved( Mouse *mouse );

	// Position functions.
	virtual void	setPosition( float x, float y );
	virtual void	setX( float x );
	virtual void	setY( float y );
	virtual float	getX() const;
	virtual float	getY() const;

	// Size functions.
	virtual int		getWidth() const;
	virtual int		getHeight() const;
	void			setSize( int width, int height );

private:
	float x_, y_;
	int width_, height_;
};