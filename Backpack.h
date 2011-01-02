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
		Container* parent );
	virtual ~Backpack();

	// Removing.
	void removeSlots();

	// Drawing functions.
	void	draw( DirectX* directX );

	// Position handling.
	virtual void updatePosition();

	// Mouse input handling.
	virtual bool mouseEvent( Mouse *mouse, EMouseEvent eventType );
	virtual void mouseClicked( Mouse *mouse, Component *component );
	virtual void mouseReleased( Mouse *mouse, Component *component );
	virtual void mouseMoved( Mouse *mouse, Component *component );

	// Page viewing functions.
	void nextPage();
	void prevPage();
	void moveCamera();

	// Inventory handling.
	bool			isLoaded() const;
	void			setLoaded();
	void			select( Slot *slot );

private:
	slotVector selected_;
	bool isLoaded_;

	// Backpack navigation.
	int page_, excludedPage_;
	int pageDelay;
	float cameraX_, cameraDest_, cameraSpeed_;
};