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
	void update();
	void draw( Graphics2D* graphics );

	// Getting the slot.
	Slot* get_slot( void ) const;
	void set_slot( Slot* slot );

	// Status functions.
	void set_selected( bool is_selected );
	bool is_selected( void ) const;
	void set_enabled( bool is_enabled );
	bool is_enabled( void ) const;

	// Precaching class-wide resources.
	static void	precache( Graphics2D* graphics );
	static void release( void );

private:
	
	// Font and text for indicating equipped.
	static IFont* equipped_font_;
	static Text* equipped_text_;

private:

	// Rounded rectangle pointers.
	RoundedRectangle* slot_rectangle_;

	// Item icon image.
	Image* item_image_;

	Slot* slot_;
	bool is_selected_;
	bool is_enabled_;

};

#endif // SLOT_VIEW_H