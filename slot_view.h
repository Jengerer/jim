#ifndef SLOT_VIEW_H
#define SLOT_VIEW_H

#include "directx.h"
#include "draggable.h"
#include "image.h"
#include "item.h"
#include "rounded_rectangle.h"
#include "slot.h"
#include "text.h"

class SlotView: public Container
{

public:

	SlotView( Slot* slot );

	// Over-ridden drawing function.
	void Update();
	void OnDraw( DirectX* directX );

	// Getting the slot.
	Slot* GetSlot() const;

	// Select type.
	void SetSelected( bool isSelected );
	bool IsSelected() const;

	// Precaching class-wide resources.
	static void	Precache( DirectX *directX );
	static void Release();

private:

	// TODO: Maybe come up with a better set up, mixing together.
	static RoundedRectangle* emptySlot_;
	static RoundedRectangle* normalSlot_;
	static RoundedRectangle* normalSelected_;
	static RoundedRectangle* vintageSlot_;
	static RoundedRectangle* vintageSelected_;
	static RoundedRectangle* genuineSlot_;
	static RoundedRectangle* genuineSelected_;
	
	static Font* equippedFont_;
	static Text* equippedText_;

private:

	Image*	slotImage_;
	Image*	itemImage_;

	Slot*	slot_;
	bool	isSelected_;

};

#endif // SLOT_VIEW_H