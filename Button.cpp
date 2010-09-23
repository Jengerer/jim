#include "Button.h"

Texture* Button::m_lpTexture = NULL;

Button::Button(const string& newCaption): Drawable(0.0f, 0.0f)
{
	m_captionString = newCaption;
}

Button::Button(const string& newCaption, const float xNew, const float yNew): Drawable(xNew, yNew)
{
	m_captionString = newCaption;
}

void Button::drawObject(DirectX* pDirectX)
{
	// Draw button base.
	pDirectX->drawTexture(m_lpTexture, m_fX, m_fY);
	
	// Draw text in center.
	RECT buttonRect;
	buttonRect.left = (long)m_fX;
	buttonRect.top = (long)m_fY;
	buttonRect.right = buttonRect.left + getWidth();
	buttonRect.bottom = buttonRect.top + getHeight();
	pDirectX->drawText(m_captionString, &buttonRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER, D3DCOLOR_XRGB(0, 0, 0));
}

int Button::getWidth()
{
	return m_lpTexture->getWidth();
}

int Button::getHeight()
{
	return m_lpTexture->getHeight();
}