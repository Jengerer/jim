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
	Component( float x = 0.0f, float y = 0.0f );
	virtual ~Component();

	// Mouse handling functions.
	void addMouseListener( MouseListener* mouseListener );
	void callMouseListener( Mouse* mouse, EMouseEvent eventType );

	// Position functions.
	virtual void	setPosition( float x, float y );
	virtual float	getX() const;
	virtual float	getY() const;

	// Virtual size getters.
	virtual int		getWidth() const = 0;
	virtual int		getHeight() const = 0;

private:
	float x_, y_;
	vector<MouseListener*> mouseListeners_;
};