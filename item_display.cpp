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
ItemDisplay::ItemDisplay( ItemSchema* schema ) : RoundedRectangleContainer( ITEM_DISPLAY_PADDING )
{
	// Set default attributes.
	set_alpha( 0 );
	set_item( nullptr );
	set_active( false );

	schema_ = schema;

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
	// Set up rectangle container.
	if (!RoundedRectangleContainer::initialize())
	{
		return false;
	}
	RoundedRectangle* rectangle = get_rounded_rectangle();
	rectangle->set_radius( ITEM_DISPLAY_RADIUS );
	rectangle->set_colour( &ITEM_DISPLAY_COLOUR );

	// Create layout.
	if (!JUTIL::BaseAllocator::allocate( &text_layout_ )) {
		return false;
	}
	text_layout_ = new (text_layout_) JUI::VerticalLayout();
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
	if (!text_layout_->add( name_text_ )) {
		JUTIL::BaseAllocator::destroy( name_text_ );
        return false;
	}
    name_text_->set_text_formatting( DT_CENTER );

    // Create info text element.
    if (!JUTIL::BaseAllocator::allocate( &info_text_ )) {
        return false;
    }
	info_text_ = new (info_text_) JUI::WrappedText( info_font_, ITEM_DISPLAY_TEXT_WIDTH );
    if (!text_layout_->add( info_text_ )) {
        JUTIL::BaseAllocator::destroy( info_text_ );
        return false;
    }

	// Pack so we can create a temporary rectangle.
	pack();
    return true;
}

/*
 * Update item display.
 */
bool ItemDisplay::update_display( void )
{
	// Alter display based on quality.
	name_text_->set_colour( item_->get_quality_colour() );

	JUTIL::DynamicString item_name;
	if(item_->get_quality() != k_EItemQuality_Common && !item_->is_renamed()){
		const JUTIL::String* quality_name;
		if(item_->get_quality() == k_EItemQuality_Strange){
			quality_name = schema_->get_kill_eater_type( item_->get_strange_type(0) )->get_levels()->find_level( item_->get_strange_number(0) )->get_prefix();
		}else{
			quality_name = schema_->get_quality_name( item_->get_quality() );
		}
		if (quality_name != nullptr) {
			if (!item_name.write( "%s ", quality_name->get_string() )) {
				return false;
			}
		}
	}

	if (!item_name.write( "%s", item_->get_name()->get_string()) ) {
		return false;
	}

	// Get item craft number
	const uint32 item_craft_number= item_->get_craft_number();
	if(item_craft_number != 0){
		if (!item_name.write( " #%u", item_craft_number )) {
			return false;
		}
	}
	set_name( &item_name );

	// Build information text.
    JUTIL::DynamicString information;
    if (!information.write( "Level %u", static_cast<uint16>(item_->get_level()) )) {
        return false;
    }

	// Load strange kills.
	for( uint32 i = 0; i < 5; ++i){
		uint32 item_strange_kills = item_->get_strange_number(i);
		if (item_strange_kills != FL_ITEM_NOT_STRANGE) {
			const KillEaterType* type = schema_->get_kill_eater_type( item_->get_strange_type(i) );
			if (type != nullptr && type->get_description() != nullptr) {
				if (!information.write( "\n%s: %u", type->get_description()->get_string(), item_strange_kills )) {
					return false;
				}
			}
		}
	}

	// Add crate series display.
	uint32 item_value = item_->get_crate_number();
	if (item_value != FL_ITEM_NOT_CRATE) {
		if (!information.write( "\nCrate Series %u", item_value )) {
			return false;
		}
	}

	// Write uses for expendable tools.
	item_value = item_->get_tool_type();
	if (item_value == TOOL_DUEL_MINIGAME || item_value == TOOL_NOISE_MAKER) {
		item_value = item_->get_count();
		if (item_value == FL_ITEM_UNLIMITED_COUNT) {
			if (!information.write( "\nUses: Unlimited" )) {
				return false;
			}
		} else {
			if (!information.write( "\nUses: %u", item_value )) {
				return false;
			}
		}
	}

#if defined( _DEBUG )

	// Write origin type.
	const JUTIL::DynamicString* origin_name = schema_->get_origin_name( item_->get_origin() );
	if (origin_name != nullptr) {
		if (!information.write( "\nOrigin: %s", origin_name->get_string() )) {
		    return false;
		}
	}

	// Add debugging information.
    if (!information.write( "\nFlags: %x", item_->get_inventory_flags() )) {
        return false;
    }

	if (!information.write( "\nTool: %u", item_->get_tool_type() )) {
        return false;
    }

#endif

	// Output if item is not tradable.
	if (!item_->is_tradable()) {
        if (!information.write( "\n\n(Not Tradable)" )) {
            return false;
        }
	}

	// Get buffer of correct size.
    JUTIL::DynamicWideString wide_string;
	int wide_size = MultiByteToWideChar( CP_UTF8, 0, information.get_string(), information.get_length(), nullptr, 0 );
    if (!wide_string.set_length( wide_size )) {
        return false;
    }

	// Convert to wide string.
	MultiByteToWideChar( CP_UTF8, 0, information.get_string(), information.get_length(), wide_string.get_string(), wide_size );
    info_text_->set_text( &wide_string );
	pack();
    return true;
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
	text_layout_->pack( ITEM_DISPLAY_SPACING, JUI::ALIGN_CENTER );
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
	if ((item_ != item) || !is_active()) {
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