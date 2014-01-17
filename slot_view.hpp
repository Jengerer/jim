#ifndef SLOT_VIEW_HPP
#define SLOT_VIEW_HPP

#include <jui/layout/constrained_container.hpp>
#include <jui/gfx/font_interface.hpp>
#include <jui/gfx/font_factory.hpp>
#include <jui/gfx/image.hpp>
#include <jui/gfx/cropped_image.hpp>
#include <jui/gfx/text.hpp>

#include "item.hpp"
#include "rounded_rectangle.hpp"
#include "slot.hpp"

class SlotView: public JUI::ConstrainedContainer
{

public:

	// Slot display attributes.
	static const unsigned int ENABLED_ALPHA  = 255;
	static const unsigned int DISABLED_ALPHA = 50;
	static const unsigned int DRAG_ALPHA     = 200;
	static const unsigned int SLOT_RADIUS    = 7;

	// Initialization and construction.
	SlotView( void );
    bool initialize( void );
	void update( const Slot* slot );
	void update_item( const Slot* slot );

	// Precaching class-wide resources.
	static bool precache( JUI::Graphics2D* graphics );
	static void release( void );

private:
	
	// Font and text for indicating equipped.
	static JUI::FontInterface* equipped_font_;
	static JUI::Text* equipped_text_;

	static JUI::FontInterface* crate_font_;
	static JUI::Text* crate_text_;

private:

	RoundedRectangle* slot_rectangle_;
	JUI::Image* image_;

};

#endif // SLOT_VIEW_HPP