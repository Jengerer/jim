#include "MouseListener.h"

MouseListener::MouseListener()
{
	setWindow(NULL);
}

MouseListener::MouseListener(Window* pWindow)
{
	setWindow(pWindow);
}

void MouseListener::setWindow(Window* pWindow)
{
	m_pWindow = pWindow;
}

void MouseListener::pollMouse()
{
	GetCursorPos(&m_ptMouse);
	ScreenToClient(m_pWindow->getHandle(), &m_ptMouse);
}

void MouseListener::setPosition(int xNew, int yNew)
{
	m_ptMouse.x = xNew;
	m_ptMouse.y = yNew;
}

int MouseListener::getX() const
{
	return (int)m_ptMouse.x;
}

int MouseListener::getY() const
{
	return (int)m_ptMouse.y;
}

void MouseListener::getPosition(int* pX, int* pY) const
{
	*pX = m_ptMouse.x;
	*pY = m_ptMouse.y;
}