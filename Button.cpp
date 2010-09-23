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
	// Check for collision.
	D3DCOLOR thisColour;
	if (mouseTouching(pDirectX))
	{
		thisColour = D3DCOLOR_ARGB(255, 180, 80, 15);
	} else
	{
		thisColour = D3DCOLOR_ARGB(255, 255, 255, 255);
	}

	// Draw button base.
	pDirectX->drawTexture(m_lpTexture, m_fX, m_fY, thisColour);
	
	// Draw text in center.
	RECT buttonRect;
	buttonRect.left = (long)m_fX;
	buttonRect.top = (long)m_fY;
	buttonRect.right = buttonRect.left + getWidth();
	buttonRect.bottom = buttonRect.top + getHeight();
	pDirectX->drawText(m_captionString, &buttonRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER, D3DCOLOR_XRGB(0, 0, 0));
}

const Texture* Button::getTexture() const
{
	return m_lpTexture;
}