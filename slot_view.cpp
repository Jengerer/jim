#include "slot_view.hpp"

// Slot layout attributes.
const unsigned int SLOT_WIDTH	= 80;
const unsigned int SLOT_HEIGHT	= 60;
const unsigned int ITEM_SIZE	= 60;

// Slot stroke attributes.
const unsigned int SLOT_STROKE_WIDTH = 3;

// Slot colour attributes.
const JUI::Colour SLOT_NORMAL_COLOUR( 60, 53, 46 );
const JUI::Colour SLOT_SELECTED_COLOUR( 90, 80, 72 ); 
const double BACKGROUND_VALUE_RATIO = 2.8;

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
	image_ = nullptr;

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

	// Add image for item.
	if (!JUTIL::BaseAllocator::allocate( &image_ )) {
		return false;
	}
	image_ = new (image_) JUI::Image( nullptr );
	if (!add( image_ )) {
		JUTIL::BaseAllocator::destroy( image_ );
		return false;
	}
	image_->set_size( ITEM_SIZE, ITEM_SIZE );
	set_constraint( image_, (SLOT_WIDTH - ITEM_SIZE) / 2, 0 );
	return true;
}

/*
 * Set the texture of the item being shown here.
 */
void SlotView::set_item_texture( const JUI::Texture* texture )
{
    image_->set_texture( texture );
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
