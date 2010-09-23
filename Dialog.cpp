#include "Dialog.h"

Texture* Dialog::m_lpTexture = NULL;

Dialog::Dialog(const string& newMsg)
{
	setMessage(newMsg);
}

void Dialog::drawObject(DirectX* pDirectX)
{
	// Draw the background texture.
	pDirectX->drawTexture(m_lpTexture, m_fX, m_fY);

	// Center the text.
	RECT textRect;
	textRect.left = (long)m_fX + PADDING;
	textRect.top = (long)m_fY + PADDING;
	textRect.right = (long)m_fX + getWidth() - PADDING;
	textRect.bottom = (long)m_fY + getHeight() - PADDING;

	// Draw it.
	pDirectX->drawText(m_dialogMsg, &textRect, DT_CENTER, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void Dialog::mouseInteract()
{
	// Nothing yet.
}

int Dialog::getWidth()
{
	return m_lpTexture->getWidth();
}

int Dialog::getHeight()
{
	return m_lpTexture->getHeight();
}

void Dialog::setMessage(const string& newMsg)
{
	m_dialogMsg = newMsg;
}

void Dialog::appendMessage(const string& addMsg)
{
	m_dialogMsg += addMsg;
}