#include "slot_view.hpp"

// Slot layout attributes.
const unsigned int SLOT_PADDING					= 5;
const unsigned int SLOT_SPACING					= 5;
const unsigned int SLOT_WIDTH					= 70;
const unsigned int SLOT_HEIGHT					= 60;
const unsigned int ITEM_SIZE					= 60;

// Slot stroke attributes.
const unsigned int SLOT_STROKE_WIDTH			= 2;
const JUI::Colour SLOT_STROKE_NORMAL_COLOUR( 248, 212, 0 );

// Slot colour attributes.
const JUI::Colour SLOT_NORMAL_COLOUR( 60, 53, 46 );
const JUI::Colour SLOT_SELECTED_COLOUR( 90, 80, 72 );

// Slot display attributes.
const unsigned int ENABLED_ALPHA				= 255;
const unsigned int DISABLED_ALPHA				= 50;
const unsigned int DRAG_ALPHA					= 185;
const unsigned int SLOT_RADIUS					= 5;

// Class-wide font/text resources.
JUI::FontInterface* SlotView::equipped_font_		= nullptr;
JUI::Text* SlotView::equipped_text_					= nullptr;

JUI::FontInterface* SlotView::crate_font_			= nullptr;
JUI::Text* SlotView::crate_text_					= nullptr;

// Equipped text attributes.
const JUTIL::ConstantString EQUIPPED_FONT_FACE	= "fonts/tf2build.ttf";
const unsigned int EQUIPPED_FONT_SIZE			= 6;
const bool EQUIPPED_FONT_BOLDED					= false;
const int EQUIPPED_PADDING					= 5;
const JUTIL::ConstantString EQUIPPED_TEXT       = "EQUIPPED";

// Crate text attributes.
const JUTIL::ConstantString CRATE_FONT_FACE	= "fonts/tf2build.ttf";
const unsigned int CRATE_FONT_SIZE			= 12;
const bool CRATE_FONT_BOLDED					= false;
const int CRATE_PADDING					= 5;

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
    item_image_ = nullptr;

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

	// Create item image.
    if (!JUTIL::BaseAllocator::allocate( &item_image_ )) {
        return false;
    }
	item_image_ = new (item_image_) JUI::Image( nullptr );
    if (!add( item_image_ )) {
        JUTIL::BaseAllocator::destroy( item_image_ );
        return false;
    }
	item_image_->set_size( ITEM_SIZE, ITEM_SIZE );

    // Align in center.
    int center_x = (SLOT_WIDTH - ITEM_SIZE) / 2;
    int center_y = (SLOT_HEIGHT - ITEM_SIZE) / 2;
	set_constraint( item_image_, center_x, center_y );
    return true;
}

/*
 * Update rectangle/image based on item.
 */
void SlotView::update( void )
{
	// Update item view.
	Slot* slot = get_slot();
	if (slot->has_item()) {
		Item* item = slot->get_item();
		item_image_->set_texture( item->get_texture() );
		slot_rectangle_->set_stroke( SLOT_STROKE_WIDTH, item->get_quality_colour() );
		slot_rectangle_->set_colour( is_selected() ? &SLOT_SELECTED_COLOUR : &SLOT_NORMAL_COLOUR );
	}
	else {
		item_image_->set_texture( nullptr );
		slot_rectangle_->set_stroke( 0, &SLOT_STROKE_NORMAL_COLOUR );
		slot_rectangle_->set_colour( &SLOT_NORMAL_COLOUR );
	}

	// Dim image when disabled.
	if (!is_enabled()) {
		//set_alpha( DISABLED_ALPHA );
		item_image_->set_alpha( DISABLED_ALPHA );
	}
	else {
		//set_alpha( ENABLED_ALPHA );
		item_image_->set_alpha( ENABLED_ALPHA );
	}
}

/*
 * Draw the slot view.
 */
void SlotView::draw( JUI::Graphics2D* graphics )
{
	update();
	Container::draw( graphics );

	// Draw equipped text.
	if (slot_->has_item()) {
		Item* item = slot_->get_item();
		if (item->is_equipped()) {
            int equipped_text_x = get_x() + get_width() - equipped_text_->get_width() - EQUIPPED_PADDING;
            int equipped_text_y = get_y() + get_height() - equipped_text_->get_height() - EQUIPPED_PADDING;
			equipped_text_->set_position( equipped_text_x, equipped_text_y );
			equipped_text_->set_text( &EQUIPPED_TEXT );
			equipped_text_->draw( graphics );
		}
		uint32 item_value = item->get_crate_number();
		JUTIL::DynamicString crate_string;
		if (item_value != FL_ITEM_NOT_CRATE) {
            int crate_text_x = get_x() + get_width() - crate_text_->get_width() - CRATE_PADDING;
            int crate_text_y = get_y() + get_height() - crate_text_->get_height() - CRATE_PADDING;
			crate_text_->set_position( crate_text_x, crate_text_y );
			crate_string.write("%u", item_value);
			crate_text_->set_text(&crate_string);
			crate_text_->draw( graphics );
		}
	}
}

/*
 * Get the slot this view is representing.
 */
Slot* SlotView::get_slot( void ) const
{
	return slot_;
}

/*
 * Set slot this view is representing.
 */
void SlotView::set_slot( Slot* slot )
{
	slot_ = slot;
}

/*
 * Set whether this slot is being selected.
 */
void SlotView::set_selected( bool is_selected )
{
	is_selected_ = is_selected;
}

/*
 * Check whether slot is enabled.
 */
bool SlotView::is_enabled( void ) const
{
	return is_enabled_;
}

/*
 * Set whether slot is enabled.
 */
void SlotView::set_enabled( bool is_enabled )
{
	is_enabled_ = is_enabled;
}

/*
 * Return whether this slot is selected.
 */
bool SlotView::is_selected() const
{
	return is_selected_;
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

	crate_font_ = JUI::FontFactory::create_font( &CRATE_FONT_FACE, CRATE_FONT_SIZE );
    if (crate_font_ == nullptr) {
        return false;
    }

    // Create text.
    if (!JUTIL::BaseAllocator::allocate( &equipped_text_ )) {
        return false;
    }
	equipped_text_ = new (equipped_text_) JUI::Text( equipped_font_ );
	equipped_text_->set_text( &EQUIPPED_TEXT );

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
    JUTIL::BaseAllocator::safe_destroy( &equipped_text_ );

	JUI::FontFactory::destroy_font( crate_font_ );
    JUTIL::BaseAllocator::safe_destroy( &crate_text_ );
}