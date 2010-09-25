#pragma once

#include "Texture.h"
#include "Drawable.h"
#include "Item.h"

class Slot: public Drawable
{
public:
	Slot( Item* item = NULL );

	// Drawing function.
	void drawObject( DirectX* directX );

	// Input handling.
	void onMouseEvent( MouseListener* pMouse, EMouseEvent mEvent );

	// Class-wide texture.
	const Texture* getTexture() const;
	static Texture* m_lpTexture;

	// The item stored by this slot.
	Item*	item;

private:
	bool	isActive;
};