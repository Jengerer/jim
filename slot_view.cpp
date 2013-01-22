#include "slot_view.hpp"

// Slot layout attributes.
const unsigned int SLOT_WIDTH	= 80;
const unsigned int SLOT_HEIGHT	= 60;
const unsigned int ITEM_SIZE	= 80;

// Slot stroke attributes.
const unsigned int SLOT_STROKE_WIDTH			= 3;
const JUI::Colour SLOT_STROKE_NORMAL_COLOUR( 248, 212, 0 );

// Slot colour attributes.
const JUI::Colour SLOT_NORMAL_COLOUR( 60, 53, 46 );
const JUI::Colour SLOT_SELECTED_COLOUR( 90, 80, 72 ); 
const double BACKGROUND_VALUE_RATIO = 2.8;

// Slot display attributes.
const unsigned int ENABLED_ALPHA				= 255;
const unsigned int DISABLED_ALPHA				= 50;
const unsigned int DRAG_ALPHA					= 185;
const unsigned int SLOT_RADIUS					= 7;

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

const int NUM_IMAGES = 3;

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
    size_t i;
    size_t length = NUM_IMAGES;
    for (i = 0; i < length; ++i) {
        item_images_.push( nullptr );
	}


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
	int center_x = SLOT_WIDTH - ITEM_SIZE;
	int center_y = SLOT_HEIGHT - ITEM_SIZE;
	center_x /= 2;
	center_y /= 2;

	size_t i;
	size_t length = item_images_.get_length();
    for (i = 0; i < length; ++i) {
		JUI::Image* item_image = item_images_.get(i);
		// Create item image.
		if (!JUTIL::BaseAllocator::allocate( &item_image )) {
			return false;
		}
		item_image = new (item_image) JUI::Image( nullptr );
		if (!add( item_image )) {
			JUTIL::BaseAllocator::destroy( item_image );
			return false;
		}
		item_image->set_size( ITEM_SIZE, ITEM_SIZE );

		// Align in center.
		set_constraint( item_image, center_x, center_y );
		item_images_.set(i, item_image);
	}
    return true;
}

/*
 * Update rectangle/image based on item.
 */
void SlotView::update( void )
{
	Slot* slot = get_slot();
	Item* item = nullptr;
	if (slot->has_item()) {
		item = slot->get_item();
		slot_rectangle_->set_stroke( SLOT_STROKE_WIDTH, item->get_quality_colour() );
		if( is_selected() ) {
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


	size_t i;
	size_t length = item_images_.get_length();
	for (i = 0; i < length; ++i) {
		JUI::Image* item_image = item_images_.get(i);
		// Update item view.
		if (item != nullptr) {
			if(i < item->get_texture_count()){
				item_image->set_texture( item->get_texture(i) );
				if(i != 0){
					uint32 paint_value = item->get_paint_value(i - 1);
					JUI::Colour item_colour = JUI::Colour( (paint_value >> 16) & 0xff, (paint_value >> 8) & 0xff, (paint_value >> 0) & 0xff );
					item_image->set_tint( &item_colour );
				}
			}
		}
		else {
			item_image->set_texture( nullptr );
		}

		// Dim image when disabled.
		if (!is_enabled()) {
			//set_alpha( DISABLED_ALPHA );
			item_image->set_alpha( DISABLED_ALPHA );
		}
		else {
			//set_alpha( ENABLED_ALPHA );
			item_image->set_alpha( ENABLED_ALPHA );
		}
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
			// Write text.
			if (crate_string.write( "#%u", item_value )) {
				crate_text_->set_text( &crate_string );

				// Position correctly.
				int crate_text_x = get_x() + get_width() - crate_text_->get_width() - CRATE_PADDING;
				int crate_text_y = get_y() + get_height() - crate_text_->get_height() - CRATE_PADDING;
				crate_text_->set_position( crate_text_x, crate_text_y );
				crate_text_->draw( graphics );
			}
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
