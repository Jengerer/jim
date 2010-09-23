#include "Drawable.h"

Drawable::Drawable()
{
	// At origin by default.
	setPosition(0.0f, 0.0f);
}

Drawable::Drawable(const float xNew, const float yNew)
{
	// Just set the position.
	setPosition(xNew, yNew);
}

void Drawable::setPosition(const float xNew, const float yNew)
{
	m_fX = xNew;
	m_fY = yNew;
}