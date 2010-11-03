#pragma once

#include <set>

#include "Window.h"

class Mouse
{
public:
	Mouse( Window* window );

	// Mouse control and settings.
	void setWindow( Window* window );
	void pollPosition();

	// Position functions.
	int getX() const;
	int getY() const;

private:
	Window*	window_;
	POINT	position_;
};

