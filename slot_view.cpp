#include "slot_view.h"

// Slot layout attributes.
const unsigned int SLOT_PADDING					= 5;
const unsigned int SLOT_SPACING					= 5;
const unsigned int SLOT_WIDTH					= 70;
const unsigned int SLOT_HEIGHT					= 60;
const unsigned int ITEM_SIZE					= 60;

// Slot stroke attributes.
const unsigned int SLOT_STROKE_WIDTH			= 2;
const Colour SLOT_STROKE_NORMAL_COLOUR( 248, 212, 0 );

// Slot colour attributes.
const Colour SLOT_NORMAL_COLOUR( 60, 53, 46 );
const Colour SLOT_SELECTED_COLOUR( 90, 80, 72 );

// Slot display attributes.
const unsigned int DRAG_ALPHA					= 185;
const unsigned int SLOT_RADIUS					= 5;

// Class-wide font/text resources.
IFont* SlotView::equipped_font_					= nullptr;
Text* SlotView::equipped_text_					= nullptr;

// Equipped text attributes.
const char* EQUIPPED_FONT_FACE					= "fonts/tf2build.ttf";
const unsigned int EQUIPPED_FONT_SIZE			= 6;
const bool EQUIPPED_FONT_BOLDED					= false;
const float EQUIPPED_PADDING					= 5;

SlotView::SlotView( Slot* slot )
{
	slot_ = slot;
	set_selected( false );

	// Set default rectangle.
	slot_rectangle_ = new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_NORMAL_COLOUR );
	add( slot_rectangle_ );
	set_constraint( slot_rectangle_, 0.0f, 0.0f );
	stroke_ = nullptr;

	// Image of item.
	item_image_ = new Image( nullptr );
	item_image_->set_size( ITEM_SIZE, ITEM_SIZE );
	add( item_image_ );
	set_constraint( item_image_, (SLOT_WIDTH - ITEM_SIZE) / 2.0f, (SLOT_HEIGHT - ITEM_SIZE) / 2.0f );

	// Set slot size.
	set_size( SLOT_WIDTH, SLOT_HEIGHT );
}

#include <sstream>

void SlotView::Update()
{
	// Get stroke and item texture.
	if (slot_->has_item()) {
		// Set item texture.
		Item* item = slot_->get_item();
		item_image_->set_texture( item->get_texture() );

		// Set slot colour.
		slot_rectangle_->set_colour( is_selected() ? SLOT_SELECTED_COLOUR : SLOT_NORMAL_COLOUR );
		slot_rectangle_->set_stroke( SLOT_STROKE_WIDTH, item->get_quality_colour() );
	}
	else {
		// Default colour, no stroke.
		slot_rectangle_->set_colour( SLOT_NORMAL_COLOUR );
		slot_rectangle_->set_stroke( 0, SLOT_NORMAL_COLOUR );
	}
}

void SlotView::draw( Graphics2D* graphics )
{
	Update();
	Container::draw( graphics );

	// Draw equipped text.
	if (slot_->has_item()) {
		Item* item = slot_->get_item();
		if (item->is_equipped()) {
			equipped_text_->set_position( 
				get_x() + get_width() - equipped_text_->get_width() - EQUIPPED_PADDING, 
				get_y() + get_height() - equipped_text_->get_height() - EQUIPPED_PADDING );
			equipped_text_->draw( graphics );
		}
	}
}

Slot* SlotView::get_slot() const
{
	return slot_;
}

void SlotView::set_selected( bool is_selected )
{
	is_selected_ = is_selected;
}

bool SlotView::is_selected() const
{
	return is_selected_;
}

void SlotView::precache( Graphics2D* graphics )
{
	// Create equipped font.
	equipped_font_ = FontFactory::create_font( EQUIPPED_FONT_FACE, EQUIPPED_FONT_SIZE );
	equipped_text_ = new Text( equipped_font_ );
	equipped_text_->set_text( "EQUIPPED" );
}

void SlotView::release()
{
	if (equipped_font_ != nullptr) {
		delete equipped_font_;
		equipped_font_ = nullptr;
	}

	if (equipped_text_ != nullptr) {
		delete equipped_text_;
		equipped_text_ = nullptr;
	}
}