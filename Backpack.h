#pragma once

#include "Inventory.h"
#include "Steam.h"
#include "Drawable.h"
#include "SerializedBuffer.h"

class Backpack: public Inventory, public Drawable, public Steam
{
public:
	Backpack( float x, float y,
		int width, int height,
		int pages );

	// Drawing functions.
	void	draw( DirectX* directX );
	int		getWidth() const;
	int		getHeight() const;

	// Inventory handling.
	bool	isLoaded() const;
	void	setLoaded();
	void	select( Slot* slot );

	// Input functions.
	void	onMouseEvent( MouseListener* mouseListener, EMouseEvent mEvent );

private:
	slotVector selected_;

	bool isLoaded_;
};