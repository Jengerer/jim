#ifndef SLOT_VIEW_H
#define SLOT_VIEW_H

#include <jui/constrained_container.h>
#include <jui/ifont.h>
#include <jui/font_factory.h>
#include <jui/image.h>
#include <jui/text.h>

#include "item.h"
#include "rounded_rectangle.h"
#include "slot.h"

class SlotView: public ConstrainedContainer
{

public:

	// Constructor.
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
	
	// Font and text for indicating equipped.
	static IFont* equipped_font_;
	static Text* equipped_text_;

private:

	// Rounded rectangle pointers.
	RoundedRectangle* slot_rectangle_;
	RoundedRectangle* stroke_;

	// Item icon image.
	Image* item_image_;

	Slot* slot_;
	bool is_selected_;

};

#endif // SLOT_VIEW_H