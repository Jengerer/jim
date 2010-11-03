#pragma once

#include <vector>

#include "MouseListener.h"

using namespace std;

enum EMouseEvent;
class MouseListener;
class Component {
public:
	Component( float x = 0.0f, float y = 0.0f);
	virtual ~Component();

	// Mouse handling functions.
	void addMouseListener( MouseListener* mouseListener );
	void callMouseListener( EMouseEvent eventType );

	// Position functions.
	void			setPosition( float x, float y );
	virtual float	getX() const;
	virtual float	getY() const;

	// Virtual size getters.
	virtual int		getWidth() const = 0;
	virtual int		getHeight() const = 0;

	float x, y;
private:
	vector<MouseListener*> mouseListeners_;
};