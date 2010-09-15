#pragma once

#include "Window.h"

enum EMouseState {
	MOUSE_STATE_UP,
	MOUSE_STATE_DOWN
};

class MouseListener
{
public:
	MouseListener();
	MouseListener(Window* pWindow);

	/* Mouse control and settings. */
	void setWindow(Window* pWindow);
	void setState(bool mouseDown);
	bool mouseDown() const;

	/* Mouse updating. */
	void pollMouse();
	virtual void onMouseDown() = 0;
	virtual void onMouseUp() = 0;

	/* Setting position. */
	void setPosition(int xNew, int yNew);

	/* Getting position. */
	int getX() const;
	int getY() const;
	void getPosition(int* pX, int* pY) const;

private:
	Window*		m_pWindow;
	POINT		m_ptMouse;
	EMouseState m_eMouseState;	
};