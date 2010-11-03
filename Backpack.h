#pragma once

#include "Inventory.h"
#include "Steam.h"
#include "Drawable.h"
#include "MouseListener.h"
#include "SerializedBuffer.h"

class Backpack: public Inventory, public MouseListener, public Drawable, public Steam
{
public:
	Backpack( Window* window,
		float x, float y,
		int width, int height,
		int pages );

	// Drawing functions.
	void	draw( DirectX* directX );
	int		getWidth() const;
	int		getHeight() const;

	// Mouse input handling.
	virtual void mouseClicked( Mouse *mouse, Component *component );
	virtual void mouseReleased( Mouse *mouse, Component *component );
	virtual void mouseMoved( Mouse *mouse, Component *component );

	// Inventory handling.
	bool	isLoaded() const;
	void	setLoaded();
	void	select( Slot* slot );

private:
	slotVector selected_;

	bool isLoaded_;
};