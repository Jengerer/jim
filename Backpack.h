#pragma once

#include "Inventory.h"
#include "Container.h"
#include "Steam.h"
#include "MouseListener.h"
#include "SerializedBuffer.h"

class Backpack: public Inventory, public MouseListener, public Container, public Steam
{
public:
	Backpack( float x, float y,
		int width, int height,
		int pages,
		Component* parent );
	virtual ~Backpack();

	// Drawing functions.
	void	draw( DirectX* directX );
	virtual void setPosition( float x, float y );
	virtual int	getWidth() const;
	virtual int	getHeight() const;

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