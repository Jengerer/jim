#include "slot_view.hpp"

// Slot layout attributes.
const int SLOT_WIDTH = 80;
const int SLOT_HEIGHT = 60;

// Slot stroke attributes.
const unsigned int SLOT_STROKE_WIDTH = 3;

// Slot colour attributes.
const JUI::Colour SLOT_NORMAL_COLOUR( 0x3C352E );
const JUI::Colour SLOT_SELECTED_COLOUR( 0x5A5048 ); 
const JUI::Colour SLOT_DISABLED_COLOUR( 0x141414 ); 
const double BACKGROUND_VALUE_RATIO = 2.8;

// Slot alpha attributes.
const float SLOT_VISIBLE_ALPHA = 1.0f;
const float SLOT_DISABLED_ALPHA = 0.2f;

// Class-wide font/text resources.
JUI::FontInterface* SlotView::equipped_font_	= nullptr;
JUI::FontInterface* SlotView::crate_font_		= nullptr;
JUI::Text* SlotView::equipped_text_				= nullptr;
JUI::Text* SlotView::crate_text_				= nullptr;

// Equipped text attributes.
const JUTIL::ConstantString EQUIPPED_FONT_FACE	= "fonts/tf2build.ttf";
const unsigned int EQUIPPED_FONT_SIZE			= 6;
const bool EQUIPPED_FONT_BOLDED					= false;
const int EQUIPPED_PADDING						= 5;
const JUTIL::ConstantString EQUIPPED_TEXT       = "EQUIPPED";

// Crate text attributes.
const JUTIL::ConstantString CRATE_FONT_FACE	= "fonts/tf2build.ttf";
const unsigned int CRATE_FONT_SIZE			= 10;
const bool CRATE_FONT_BOLDED				= false;
const int CRATE_PADDING						= 5;

/*
 * Slot view constructor.
 */
SlotView::SlotView( void )
{
	// Set default rectangle.
    slot_rectangle_ = nullptr;
	item_ = nullptr;

	// Set slot size.
	set_size( SLOT_WIDTH, SLOT_HEIGHT );
}

/*
 * Slot view initialization.
 */
bool SlotView::initialize( void )
{
	// Add rounded rectangle for slot.
	if (!JUTIL::BaseAllocator::allocate( &slot_rectangle_ )) {
		return false;
	}
	slot_rectangle_ = new (slot_rectangle_) RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, &SLOT_NORMAL_COLOUR );
	if (!add( slot_rectangle_ )) {
		JUTIL::BaseAllocator::destroy( slot_rectangle_ );
		return false;
	}
	slot_rectangle_->set_stroke_type( STROKE_TYPE_INNER );

	// Add image for item.
	if (!JUTIL::BaseAllocator::allocate( &item_ )) {
		return false;
	}
	if (!add( item_ )) {
		JUTIL::BaseAllocator::release( item_ );
		return false;
	}
	new (item_) ItemDecorator();
	int width = item_->get_width();
	set_constraint( item_, (SLOT_WIDTH - width) / 2, 0 );
	return true;
}

/*
 * Set the texture of the item being shown here.
 */
void SlotView::update( const Slot* slot )
{
	// Update item-dependent slot parameters.
	Item* item = slot->get_item();
	update_item( item );

	// Handle selection state.
	if (!slot->is_enabled()) {
		set_alpha( SLOT_DISABLED_ALPHA );
	}
	else {
		set_alpha( SLOT_VISIBLE_ALPHA );
	}
}

/*
 * Update item specific slot decoration.
 */
void SlotView::update_item( const Item* item )
{
	const JUI::Texture* texture;
	const JUI::Colour* stroke_colour;
	const JUI::Colour* slot_colour;
	unsigned int stroke_width;

	// Draw empty slot if beyond last item.
	if (item != nullptr) {
		texture = item->get_texture();
		stroke_colour = item->get_quality_colour();
		stroke_width = SLOT_STROKE_WIDTH;

		// Slot colour based on item selection.
		if (item->is_selected()) {
			slot_colour = &SLOT_SELECTED_COLOUR;
		}
		else {
			slot_colour = &SLOT_NORMAL_COLOUR;
		}
	}
	else {
		texture = nullptr;
		slot_colour = &SLOT_NORMAL_COLOUR;
		stroke_colour = &JUI::COLOUR_BLANK;
		stroke_width = 0;
	}
	item_->set_item( item );
	slot_rectangle_->set_colour( slot_colour );
	slot_rectangle_->set_stroke( stroke_width, stroke_colour );
}

/*
 * Precache font and text object.
 */
bool SlotView::precache( JUI::Graphics2D* graphics )
{
	// Create equipped font.
	equipped_font_ = JUI::FontFactory::create_font( &EQUIPPED_FONT_FACE, EQUIPPED_FONT_SIZE );
    if (equipped_font_ == nullptr) {
        return false;
    }

	// Create crate number font.
	crate_font_ = JUI::FontFactory::create_font( &CRATE_FONT_FACE, CRATE_FONT_SIZE );
    if (crate_font_ == nullptr) {
        return false;
    }

    // Create equipped text.
    if (!JUTIL::BaseAllocator::allocate( &equipped_text_ )) {
        return false;
    }
	equipped_text_ = new (equipped_text_) JUI::Text( equipped_font_ );
	equipped_text_->set_text( &EQUIPPED_TEXT );

	// Create crate text object.
	// TODO: Move to crate-specific item view.
	if (!JUTIL::BaseAllocator::allocate( &crate_text_ )) {
        return false;
    }
	crate_text_ = new (crate_text_) JUI::Text( crate_font_ );
    return true;
}

/*
 * Release font/text.
 */
void SlotView::release( void )
{
    JUI::FontFactory::destroy_font( equipped_font_ );
	JUI::FontFactory::destroy_font( crate_font_ );
    JUTIL::BaseAllocator::safe_destroy( &equipped_text_ );
    JUTIL::BaseAllocator::safe_destroy( &crate_text_ );
}
