#include "slot_view.hpp"

// Slot layout attributes.
const unsigned int SLOT_PADDING	= 5;
const unsigned int SLOT_SPACING	= 5;
const unsigned int SLOT_WIDTH	= 70;
const unsigned int SLOT_HEIGHT	= 60;
const unsigned int ITEM_SIZE	= 60;

// Slot stroke attributes.
const unsigned int SLOT_STROKE_WIDTH			= 2;
const JUI::Colour SLOT_STROKE_NORMAL_COLOUR( 248, 212, 0 );

// Slot colour attributes.
const JUI::Colour SLOT_NORMAL_COLOUR( 60, 53, 46 );
const JUI::Colour SLOT_SELECTED_COLOUR( 90, 80, 72 ); 
const double BACKGROUND_VALUE_RATIO = 2.8;

// Slot display attributes.
const unsigned int ENABLED_ALPHA				= 255;
const unsigned int DISABLED_ALPHA				= 50;
const unsigned int DRAG_ALPHA					= 185;
const unsigned int SLOT_RADIUS					= 5;

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
SlotView::SlotView( Slot* slot )
{
	set_slot( slot );
	set_selected( false );
	set_enabled( true );

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
    // Create slot rectangle.
    if (!JUTIL::BaseAllocator::allocate( &slot_rectangle_ )) {
        return false;
    }
	slot_rectangle_ = new (slot_rectangle_) RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, &SLOT_NORMAL_COLOUR );
	if (!add( slot_rectangle_ )) {
        JUTIL::BaseAllocator::destroy( slot_rectangle_ );
        return false;
    }

	int center_x = (SLOT_WIDTH - ITEM_SIZE) / 2;
	int center_y = (SLOT_HEIGHT - ITEM_SIZE) / 2;
	// Create item image.
	if (!JUTIL::BaseAllocator::allocate( &image_ )) {
		return false;
	}
	image_ = new (image_) JUI::Image( nullptr );
	if (!add( image_ )) {
		JUTIL::BaseAllocator::destroy( image_ );
		return false;
	}
	image_->set_size( ITEM_SIZE, ITEM_SIZE );

	// Align in center.
	set_constraint( image_, center_x, center_y );
	return true;
}

/*
 * Update slot display based on item.
 */
bool SlotView::update( Slot* slot )
{
	Item* item = slot->get_item();
	if (item != nullptr) {
		// Update item image.
		image_->set_texture( item->get_texture() );
		item = slot->get_item();
		slot_rectangle_->set_stroke( SLOT_STROKE_WIDTH, item->get_quality_colour() );
		if(is_selected()) {
			JUI::Colour quality_colour_background = (*item->get_quality_colour());
			quality_colour_background.set_value( quality_colour_background.get_value() / BACKGROUND_VALUE_RATIO );
			slot_rectangle_->set_colour( &quality_colour_background );
		}else{
			slot_rectangle_->set_colour( &SLOT_NORMAL_COLOUR );
		}
	}
	else {
		slot_rectangle_->set_stroke( 0, &SLOT_STROKE_NORMAL_COLOUR );
		slot_rectangle_->set_colour( &SLOT_NORMAL_COLOUR );
	}
}

/*
 * Draw the slot view.
 */
void SlotView::draw( JUI::Graphics2D* graphics )
{
	Container::draw( graphics );
}

/*
 * Check whether slot is enabled.
 */
bool SlotView::is_enabled( void ) const
{
	return is_enabled_;
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
