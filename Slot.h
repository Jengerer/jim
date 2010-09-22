#pragma once

#include "Texture.h"
#include "Drawable.h"
#include "Item.h"

class Slot: public Drawable
{
public:
	Slot();
	Slot(Item* whichItem);

	// Drawing function.
	void drawObject(DirectX* pDirectX);

	// Size getters.
	int getWidth();
	int getHeight();

	static Texture* m_slotTexture;

	// The item stored by this slot.
	Item*	m_pItem;
};