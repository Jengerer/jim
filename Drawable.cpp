#include "Drawable.h"

Drawable::Drawable()
{
	// Just set the position.
	m_fX = m_fY = 0.0f;
}

Drawable::Drawable(const float xNew, const float yNew)
{
	m_fX = xNew;
	m_fY = yNew;
}