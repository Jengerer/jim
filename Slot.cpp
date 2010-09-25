#include "Slot.h"

// Initialize the texture.
Texture* Slot::m_lpTexture = NULL;

Slot::Slot(Item* whichItem)
{
	m_pItem = whichItem;
	m_isActive = false;
}

Slot::Slot()
{
	m_pItem = NULL;
	m_isActive = false;
}

void Slot::drawObject(DirectX* directX)
{
	// Check for collision.
	D3DCOLOR thisColour = (m_isActive ? D3DCOLOR_ARGB(255, 200, 200, 200) : D3DCOLOR_ARGB(255, 100, 100, 100));

	// Draw the slot texture here.
	directX->drawTexture(m_lpTexture, m_fX, m_fY, thisColour);

	if (m_pItem != NULL)
	{
		// Move it to the center of this slot.
		m_pItem->m_fX = m_fX + getWidth()/2 - m_pItem->getWidth()/2;
		m_pItem->m_fY = m_fY + getHeight()/2 - m_pItem->getHeight()/2;

		// Draw it.
		m_pItem->drawObject(directX);
	}
}

void Slot::onMouseEvent(MouseListener* pMouse, EMouseEvent mEvent)
{
	// Check collision.
	m_isActive = mouseTouching(pMouse);
}

const Texture* Slot::getTexture() const
{
	return m_lpTexture;
}