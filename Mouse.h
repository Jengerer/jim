#pragma once

#include <vector>

#include "Window.h"
#include "MouseListener.h"
#include "Component.h"

using namespace std;

enum EMouseEvent;
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

	// Call to components.
	void triggerEvent( EMouseEvent eventType );

	static void createComponents();
	static void addComponent( Component* component );
	static void removeComponent( Component* component );
	static void clearComponents();

private:
	static vector<Component*>* components_;
	Window*	window_;
	POINT	position_;
};

