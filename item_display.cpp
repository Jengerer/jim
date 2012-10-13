#include <jui/gfx/font_interface.hpp>
#include <jui/gfx/font_factory.hpp>

#include "item_display.hpp"

// Title style attributes.
const JUTIL::ConstantString ITEM_DISPLAY_TITLE_FONT_FACE = "fonts/tf2build.ttf";
const unsigned int ITEM_DISPLAY_TITLE_FONT_SIZE	= 13;

// Information style attributes.
const JUTIL::ConstantString ITEM_DISPLAY_INFO_FONT_FACE = "fonts/tf2secondary.ttf";
const unsigned int ITEM_DISPLAY_INFO_FONT_SIZE = 11;

// Display colour.
const JUI::Colour& ITEM_DISPLAY_COLOUR = JUI::COLOUR_BLACK;

// Name fonts.
JUI::FontInterface *ItemDisplay::name_font_ = nullptr;
JUI::FontInterface *ItemDisplay::info_font_ = nullptr;

// Item dipslay layout attributes.
const unsigned int ITEM_DISPLAY_PADDING = 20;
const unsigned int ITEM_DISPLAY_SPACING = 5;
const unsigned int ITEM_DISPLAY_RADIUS = 5;
const int ITEM_DISPLAY_TEXT_WIDTH = 200;
const int ITEM_DISPLAY_ALPHA_SPEED = 30;
const int ITEM_DISPLAY_ALPHA_MAX = 210;

/*
 * Item display constructor.
 */
ItemDisplay::ItemDisplay( void ) : RoundedRectangleContainer( ITEM_DISPLAY_PADDING )
{
	// Set default attributes.
	set_alpha( 0 );
	set_item( nullptr );
	set_active( false );

	// Null objects.
	name_text_ = nullptr;
	info_text_ = nullptr;
	text_layout_ = nullptr;
}

/*
 * Item display destructor.
 */
ItemDisplay::~ItemDisplay( void )
{
	// Nothing.
}

/*
 * Initialize UI elements for display.
 */
bool ItemDisplay::initialize( void )
{
	// Set up rectangle.
	if (!RoundedRectangleContainer::initialize())
	{
		return false;
	}

	// Set up rounded rectangle.
	RoundedRectangle* rectangle = get_rounded_rectangle();
	rectangle->set_radius( ITEM_DISPLAY_RADIUS );
	rectangle->set_colour( &ITEM_DISPLAY_COLOUR );

	// Create layout.
	if (!JUTIL::BaseAllocator::allocate( &text_layout_ )) {
		return false;
	}
	text_layout_ = new (text_layout_) JUI::VerticalLayout();
	text_layout_->set_spacing( ITEM_DISPLAY_SPACING );
	if (!add( text_layout_ )) {
		JUTIL::BaseAllocator::destroy( text_layout_ );
		return false;
	}
	set_content( text_layout_ );
	
	// Create name text element.
	if (!JUTIL::BaseAllocator::allocate( &name_text_ )) {
		return false;
	}
	name_text_ = new (name_text_) JUI::WrappedText( name_font_, ITEM_DISPLAY_TEXT_WIDTH );
	if (!add( name_text_ )) {
		JUTIL::BaseAllocator::destroy( name_text_ );
	}

	name_text_->set_text_formatting( DT_CENTER );
	info_text_ = new JUI::WrappedText( info_font_, ITEM_DISPLAY_TEXT_WIDTH );
	info_text_->set_text_formatting( DT_CENTER );

	// Add to layout.
	text_layout_ = new JUI::VerticalLayout();
	text_layout_->set_spacing( ITEM_DISPLAY_SPACING );
	text_layout_->add( name_text_ );
	text_layout_->add( info_text_ );

	// Pack so we can create a temporary rectangle.
	add( text_layout_ );
	set_content( text_layout_ );
	pack();
}

/*
 * Update item display.
 */
bool ItemDisplay::update_display( void )
{
	// Alter display based on quality.
	name_text_->set_colour( item_->get_quality_colour() );
	set_name( item_->get_name() );

	// Build information text.
    JUTIL::DynamicString information;
    if (!information.write( "LEVEL %u", static_cast<uint16>(item_->get_level()) )) {
        return false;
    }

#if defined( _DEBUG )
	// Add debugging information.
    if (!information.write( "\nFLAGS: %x", item_->get_flags() )) {
        return false;
    }
	if (!information.write( "\nORIGIN: %u", item_->get_origin() )) {
        return false;
    }
	for (size_t i = 0; i < item_->get_attribute_count(); i++) {
		const Attribute* attribute = item_->get_attribute_at( i );

        // Write string if attribute has description.
		if (attribute->has_description()) {
            if (!information.write( "\n%s", attribute->get_description() ))
            {
                return false;
            }
		}
	}
#endif

	// Output if item is not tradable.
	if (!item_->is_tradable()) {
        if (!information.write( "\n\n(Not Tradable)" )) {
            return false;
        }
	}

	// Get buffer of correct size.
	int wide_size = MultiByteToWideChar( CP_UTF8, 0, information.get_string(), information.get_length(), nullptr, 0 );
	wchar_t* wide_buffer;
    if (!JUTIL::BaseAllocator::allocate_array( &wide_buffer, wide_size )) {
        return false;
    }

	// Convert to wide string.
	if (wide_buffer != nullptr) {
		MultiByteToWideChar( CP_UTF8, 0, information.get_string(), information.get_length(), wide_buffer, wide_size );
        JUTIL::ConstantWideString wide_information( wide_buffer, wide_size );
		info_text_->set_text( &wide_information );

		// Delete wide string.
        JUTIL::BaseAllocator::release( wide_buffer );
	}

	pack();
}

/*
 * Fade in/out based on activity.
 */
void ItemDisplay::update_alpha( void )
{
	if (is_active()) {
		// Increase and clamp alpha if active.
		int alpha = get_alpha() + ITEM_DISPLAY_ALPHA_SPEED;
		if (alpha > ITEM_DISPLAY_ALPHA_MAX) {
			alpha = ITEM_DISPLAY_ALPHA_MAX;
		}

		set_alpha( alpha );
	}
	else {
		// Decrease alpha if inactive.
		set_alpha( get_alpha() - ITEM_DISPLAY_ALPHA_SPEED );
	}
}

/*
 * Pack item display layout.
 */
void ItemDisplay::pack( void )
{
	text_layout_->pack();
	RoundedRectangleContainer::pack();
}

/*
 * Get item shown by display.
 */
const Item* ItemDisplay::get_item( void ) const
{
	return item_;
}

/*
 * Set item to be described by display.
 * Returns true if display updated successfully, false otherwise.
 */
bool ItemDisplay::set_item( const Item *item )
{
	if (item_ != item) {
		item_ = item;
		if (item != nullptr) {
			set_active( true );
			return update_display();
		}
		else {
			set_active( false );
		}
	}

    return true;
}

/*
 * Get name of item.
 */
const JUTIL::String* ItemDisplay::get_name( void ) const
{
	return item_name_;
}

/*
 * Set name of item to be displayed.
 */
void ItemDisplay::set_name( const JUTIL::String* name )
{
	item_name_ = name;
	name_text_->set_text( name );
}

/*
 * Return whether display is active.
 */
bool ItemDisplay::is_active( void ) const
{
	return is_active_;
}

/*
 * Set whether display is active.
 */
void ItemDisplay::set_active( bool is_active )
{
	is_active_ = is_active;
}

/*
 * Load item display resources.
 */
bool ItemDisplay::precache( void )
{
	// Font for item names.
	name_font_ = JUI::FontFactory::create_font( &ITEM_DISPLAY_TITLE_FONT_FACE, ITEM_DISPLAY_TITLE_FONT_SIZE );
	if (name_font_ == nullptr) {
		return false;
	}

	// Font for item description and miscellaneous information.
	info_font_ = JUI::FontFactory::create_font( &ITEM_DISPLAY_INFO_FONT_FACE, ITEM_DISPLAY_INFO_FONT_SIZE );
	if (info_font_ == nullptr) {
		JUI::FontFactory::destroy_font( name_font_ );
		return false;
	}

    return true;
}

/*
 * Release all item display resources.
 */
void ItemDisplay::release( void )
{
	JUI::FontFactory::destroy_font( name_font_ );
	JUI::FontFactory::destroy_font( info_font_ );
}