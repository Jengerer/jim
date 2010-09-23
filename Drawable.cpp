#include "Drawable.h"

Texture* Drawable::m_lpTexture = NULL;

Drawable::Drawable(const float xNew, const float yNew)
{
	// Just set the position.
	setPosition(xNew, yNew);
	m_nAlpha = 100;
}

void Drawable::setPosition(const float xNew, const float yNew)
{
	m_fX = xNew;
	m_fY = yNew;
}

bool Drawable::mouseTouching(MouseListener* pMouse) const
{
	// Get mouse position.
	int xMouse, yMouse;
	pMouse->getPosition(&xMouse, &yMouse);

	// Check collision.
	return ((xMouse >= m_fX) &&
		(xMouse <= (m_fX + getWidth())) &&
		(yMouse >= m_fY) &&
		(yMouse <= (m_fY + getHeight())));
}

int Drawable::getWidth() const
{
	return getTexture()->getWidth();
}

int Drawable::getHeight() const
{
	return getTexture()->getHeight();
}

const Texture* Drawable::getTexture() const
{
	return m_lpTexture;
}