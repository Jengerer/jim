#ifndef SLOT_VIEW_HPP
#define SLOT_VIEW_HPP

#include <jui/layout/constrained_container.hpp>
#include <jui/gfx/font_interface.hpp>
#include <jui/gfx/font_factory.hpp>
#include <jui/gfx/image.hpp>
#include <jui/gfx/cropped_image.hpp>
#include <jui/gfx/text.hpp>
#include "item_decorator.hpp"
#include "rounded_rectangle.hpp"
#include "slot.hpp"

// Slot view constants.
static const float ENABLED_ALPHA  = 1.0f;
static const float DISABLED_ALPHA = 0.2f;
static const float DRAG_ALPHA     = 0.8f;

class SlotView: public JUI::ConstrainedContainer
{

public:

	// Slot display attributes.
	static const unsigned int SLOT_RADIUS    = 7;

	// Initialization and construction.
	SlotView( void );
    bool initialize( void );
	void update( const Slot* slot );
	void update_item( const Item* item );

private:

	RoundedRectangle* slot_rectangle_;
	ItemDecorator* item_;

};

#endif // SLOT_VIEW_HPP