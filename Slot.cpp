#include "Slot.h"

// Initialize the texture.
Texture* Slot::m_slotTexture = NULL;

Slot::Slot(Item* whichItem)
{
	m_pItem = whichItem;
}

Slot::Slot()
{
	m_pItem = NULL;
}

void Slot::drawObject(DirectX* pDirectX)
{
	// Draw the slot texture here.
	pDirectX->drawTexture(m_slotTexture, m_fX, m_fY);

	if (m_pItem != NULL)
	{
		// Move it to the center of this slot.
		m_pItem->m_fX = m_fX + getWidth()/2 - m_pItem->getWidth()/2;
		m_pItem->m_fY = m_fY + getHeight()/2 - m_pItem->getHeight()/2;

		// Draw it.
		m_pItem->drawObject(pDirectX);
	}
}

int Slot::getWidth()
{
	return m_slotTexture->getWidth();
}

int Slot::getHeight()
{
	return m_slotTexture->getHeight();
}