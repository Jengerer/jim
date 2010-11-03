#pragma once

#include <vector>

#include "Window.h"
#include "Component.h"

using namespace std;

class Component;
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

	static void addComponent( Component* component );
	static void removeComponent( Component* component );
	static void clearComponents();

private:
	static vector<Component*> components_;
	Window*	window_;
	POINT	position_;
};

