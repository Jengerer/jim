#ifndef SLOT_VIEW_H
#define SLOT_VIEW_H

#include <jui/constrained_container.h>
#include <jui/font.h>
#include <jui/font_factory.h>
#include <jui/image.h>
#include <jui/text.h>

#include "item.h"
#include "rounded_rectangle.h"
#include "slot.h"

class SlotView: public ConstrainedContainer
{

public:

	SlotView( Slot* slot );

	// Over-ridden drawing function.
	void Update();
	void draw( Graphics2D* graphics );

	// Getting the slot.
	Slot* get_slot() const;

	// Select type.
	void set_selected( bool is_selected );
	bool is_selected() const;

	// Precaching class-wide resources.
	static void	precache( Graphics2D* graphics );
	static void release();

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
	bool is_selected_;

};

#endif // SLOT_VIEW_H