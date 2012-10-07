#include <jui/gfx/font_interface.hpp>
#include <jui/gfx/font_factory.hpp>

#include "item_display.hpp"

const char* ITEM_DISPLAY_TITLE_FONT_FACE = "fonts/tf2build.ttf";
const unsigned int ITEM_DISPLAY_TITLE_FONT_SIZE	= 13;

const JUI::Colour& ITEM_DISPLAY_COLOUR = JUI::COLOUR_BLACK;

const char* ITEM_DISPLAY_INFO_FONT_FACE = "fonts/tf2secondary.ttf";
const unsigned int ITEM_DISPLAY_INFO_FONT_SIZE = 11;

JUI::FontInterface *ItemDisplay::name_font_ = nullptr;
JUI::FontInterface *ItemDisplay::info_font_ = nullptr;

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
void ItemDisplay::update_display( void )
{
	// Alter display based on quality.
	name_text_->set_colour( item_->get_quality_colour() );
	set_name( item_->get_name() );

	// Build information text.
	std::stringstream infoStream;
	infoStream << "LEVEL " << static_cast<uint16>(item_->get_level());

#if defined( _DEBUG )
	// Add debugging information.
	infoStream << "\nFLAGS: " << std::hex << item_->get_flags();
	infoStream << "\nORIGIN: " << item_->get_origin();
	for (size_t i = 0; i < item_->get_attribute_count(); i++) {
		const Attribute* attrib = item_->get_attribute_at( i );
		if (attrib->has_description()) {
			infoStream << "\n" << attrib->get_description_string();
		}
	}
#endif

	// Output if item is not tradable.
	if (!item_->is_tradable()) {
		infoStream << "\n\n(Not Tradable)";
	}

	// Get resulting string.
	const std::string& text = infoStream.str();

	// Get buffer of correct size.
	int wide_size = MultiByteToWideChar( CP_UTF8, 0, text.c_str(), text.length(), nullptr, 0 );
	wchar_t* wide_buffer = new wchar_t[ wide_size ];

	// Convert to wide string.
	if (wide_buffer != nullptr) {
		MultiByteToWideChar( CP_UTF8, 0, text.c_str(), text.length(), wide_buffer, wide_size );
		info_text_->set_text( wide_buffer, wide_size );

		// Delete wide string.
		delete wide_buffer;
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

void ItemDisplay::pack( void )
{
	text_layout_->pack();
	RoundedRectangleContainer::pack();
}

const Item* ItemDisplay::get_item( void ) const
{
	return item_;
}

void ItemDisplay::set_item( const Item *item )
{
	if (item_ != item) {
		item_ = item;
		if (item != nullptr) {
			set_active( true );
			update_display();
		}
		else {
			set_active( false );
		}
	}
}

const std::string& ItemDisplay::get_name( void ) const
{
	return item_name_;
}

void ItemDisplay::set_name( const std::string& name )
{
	item_name_ = name;
	name_text_->set_text( name );
}

bool ItemDisplay::is_active( void ) const
{
	return is_active_;
}

void ItemDisplay::set_active( bool is_active )
{
	is_active_ = is_active;
}

bool ItemDisplay::precache( void )
{
	// Font for item names.
	name_font_ = JUI::FontFactory::create_font( ITEM_DISPLAY_TITLE_FONT_FACE, ITEM_DISPLAY_TITLE_FONT_SIZE );
	if (name_font_ == nullptr) {
		return false;
	}

	// Font for item description and miscellaneous information.
	info_font_ = JUI::FontFactory::create_font( ITEM_DISPLAY_INFO_FONT_FACE, ITEM_DISPLAY_INFO_FONT_SIZE );
	if (info_font_ == nullptr) {
		JUI::FontFactory::destroy_font( name_font_ );
		return false;
	}
}

/*
 * Release all item display resources.
 */
void ItemDisplay::release( void )
{
	JUI::FontFactory::destroy_font( name_font_ );
	JUI::FontFactory::destroy_font( info_font_ );
}