#ifndef SLOT_VIEW_H
#define SLOT_VIEW_H

#include <jui/layout/constrained_container.hpp>
#include <jui/gfx/font_interface.hpp>
#include <jui/gfx/font_factory.hpp>
#include <jui/gfx/image.hpp>
#include <jui/gfx/text.hpp>

#include "item.hpp"
#include "rounded_rectangle.hpp"
#include "slot.hpp"

class SlotView: public JUI::ConstrainedContainer
{

public:

	// Constructor.
	SlotView( Slot* slot );

	// Over-ridden drawing function.
	void update();
	void draw( JUI::Graphics2D* graphics );

	// Getting the slot.
	Slot* get_slot( void ) const;
	void set_slot( Slot* slot );

	// Status functions.
	void set_selected( bool is_selected );
	bool is_selected( void ) const;
	void set_enabled( bool is_enabled );
	bool is_enabled( void ) const;

	// Precaching class-wide resources.
	static void	precache( JUI::Graphics2D* graphics );
	static void release( void );

private:
	
	// Font and text for indicating equipped.
	static JUI::FontInterface* equipped_font_;
	static JUI::Text* equipped_text_;

private:

	// Rounded rectangle pointers.
	RoundedRectangle* slot_rectangle_;

	// Item icon image.
	JUI::Image* item_image_;

	Slot* slot_;
	bool is_selected_;
	bool is_enabled_;

};

#endif // SLOT_VIEW_H