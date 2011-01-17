#pragma once

#include <vector>

#include "Mouse.h"
#include "MouseListener.h"

using namespace std;

enum EMouseEvent;
class MouseListener;
class Mouse;
class Component {
public:
	Component();
	Component( float x, float y );
	virtual ~Component();

	// Mouse handling functions.
	void setMouseListener( MouseListener* mouseListener );
	virtual bool mouseEvent( Mouse* mouse, EMouseEvent eventType );

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
	MouseListener* mouseListener_;
};