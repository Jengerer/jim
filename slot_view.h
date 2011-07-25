#ifndef SLOT_VIEW_H
#define SLOT_VIEW_H

#include "constrained_container.h"
#include "font.h"
#include "font_factory.h"
#include "image.h"
#include "item.h"
#include "rounded_rectangle.h"
#include "slot.h"
#include "text.h"

class SlotView: public ConstrainedContainer
{

public:

	SlotView( Slot* slot );

	// Over-ridden drawing function.
	void Update();
	void Draw( Graphics2D* graphics );

	// Getting the slot.
	Slot* GetSlot() const;

	// Select type.
	void SetSelected( bool isSelected );
	bool IsSelected() const;

	// Precaching class-wide resources.
	static void	Precache( Graphics2D* graphics );
	static void Release();

private:

	// TODO: Maybe come up with a better set up, mixing together.
	static RoundedRectangle* normalSlot_;
	static RoundedRectangle* selectedSlot_;
	static RoundedRectangle* stroke_;
	
	static Font* equippedFont_;
	static Text* equippedText_;

private:

	Image* slotImage_;
	Image* strokeImage_;
	Image* itemImage_;

	Slot* slot_;
	bool isSelected_;

};

#endif // SLOT_VIEW_H