#include <jui/gfx/font_interface.hpp>
#include <jui/gfx/font_factory.hpp>

#include "notice.hpp"

// Static members.
JUI::FontInterface* Notice::font_						= nullptr;

// Font constants.
const char* NOTICE_FONT_FACE			= "fonts/tf2build.ttf";
const unsigned int NOTICE_FONT_SIZE		= 12;
const bool NOTICE_FONT_BOLDED			= false;

// Notice specifications.
const unsigned int NOTICE_TEXT_WIDTH	= 300;
const unsigned int NOTICE_RADIUS		= 5;
const unsigned int NOTICE_PADDING		= 20;
const int NOTICE_SPACING				= 10;
const unsigned int NOTICE_STROKE_WIDTH	= 5;
const JUI::Colour NOTICE_STROKE_COLOUR( 255, 255, 255, 100 );
const JUI::Colour& NOTICE_COLOUR = JUI::COLOUR_BLACK;

/*
 * Notice constructor.
 */
Notice::Notice( void )
{
	rounded_container_ = nullptr;
    text_ = nullptr;
}

/*
 * Notice destructor.
 */
Notice::~Notice( void )
{
}

/*
 * Allocate layout objects for displaying notice.
 */
bool Notice::initialize( const JUTIL::String* message )
{
    // Create rounded container.
    if (!JUTIL::BaseAllocator::allocate( &rounded_container_ )) {
        return false;
    }
    rounded_container_ = new (rounded_container_) RoundedRectangleContainer( NOTICE_RADIUS, NOTICE_PADDING );
        
    // Style rounded rectangle.
    RoundedRectangle* rounded_rect = rounded_container_->get_rounded_rectangle();
	rounded_rect->set_stroke( NOTICE_STROKE_WIDTH, &NOTICE_STROKE_COLOUR );
	rounded_rect->set_stroke_type( STROKE_TYPE_OUTER );
	rounded_rect->set_colour( &NOTICE_COLOUR );
	if (!add( rounded_container_ )) {
        JUTIL::BaseAllocator::destroy( rounded_container_ );
        return false;
    }
	set_constraint( rounded_container_, 0.0f, 0.0f );

	// Create layout for container.
    if (!JUTIL::BaseAllocator::allocate( &content_ )) {
        return false;
    }
	content_ = new (content_) JUI::VerticalLayout();
	content_->set_spacing( NOTICE_SPACING );
	if (!rounded_container_->add( content_ ))
    {
        JUTIL::BaseAllocator::destroy( content_ );
        return false;
    }
	rounded_container_->set_content( content_ );

	// Add default text to layout.
    if (!JUTIL::BaseAllocator::allocate( &text_ )) {
        return false;
    }
	text_ = new (text_) JUI::WrappedText( font_, NOTICE_TEXT_WIDTH );
	text_->set_text_formatting( DT_CENTER );
	if (!content_->add( text_ ))
    {
        JUTIL::BaseAllocator::destroy( text_ );
        return false;
    }

    // Set text message displayed by notice.
    if (!set_message( message ))
    {
        return false;
    }

    return true;
}

/*
 * Pack content into notice layout.
 */
void Notice::pack( void )
{
	content_->pack();
	rounded_container_->pack();
	set_size( rounded_container_->get_width(), rounded_container_->get_height() );
}

/*
 * Set message to new string.
 */
bool Notice::set_message( const JUTIL::String* message )
{
    // Remove old message.
    message_.clear();

    // Write new message to new buffer.
    if (!message_.copy( message )) {
        return false;
    }

    // Update text object to show message.
	text_->set_text( message );
	pack();
}

/*
 * Add string to message.
 */
bool Notice::append_message( const JUTIL::String* message )
{
    // Add string to buffer.
    if (!message_.write( "%s", message )) {
        return false;
    }
	set_message( &message_ );
    return true;
}

/*
 * Load notice-generic resources.
 */
bool Notice::precache( JUI::Graphics2D* graphics )
{
    // Allocate notice-generic font.
	font_ = JUI::FontFactory::create_font( NOTICE_FONT_FACE, NOTICE_FONT_SIZE );
    if (font_ == nullptr) {
        return false;
    }

    return true;
}

/*
 * Free notice-generic resource.
 */
void Notice::release( void )
{
    JUI::FontFactory::destroy_font( font_ );
}