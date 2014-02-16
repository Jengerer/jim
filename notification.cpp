#include "notification.hpp"

#include <jui/application/error_stack.hpp>
#include <jui/gfx/font_interface.hpp>
#include <jui/gfx/font_factory.hpp>
#include <jui/gfx/wrapped_text.hpp>
#include <jui/layout/horizontal_layout.hpp>

const unsigned int NOTIFICATION_ICON_SIZE	= 48;
const unsigned int NOTIFICATION_TEXT_WIDTH	= 160;
const JUI::Colour NOTIFICATION_COLOUR( 0x191919 );
const unsigned int NOTIFICATION_RADIUS		= 5;
const unsigned int NOTIFICATION_PADDING		= 10;
const unsigned int NOTIFICATION_SPACING		= 5;

const unsigned int NOTIFICATION_STROKE_SIZE	= 2;
const JUI::Colour NOTIFICATION_STROKE_COLOUR( 0xFFFFFF, 0.4f );

const JUTIL::ConstantString NOTIFICATION_FONT_FACE = "fonts/tf2build.ttf";
const unsigned int NOTIFICATION_FONT_SIZE	= 10;
const JUI::Colour& NOTIFICATION_FONT_COLOUR = JUI::COLOUR_WHITE;
JUI::FontInterface *Notification::font_ = nullptr;

/*
 * Notification constructor from message and texture.
 */
Notification::Notification( void ) : RoundedRectangleContainer( NOTIFICATION_PADDING )
{
    layout_ = nullptr;
	text_ = nullptr;
}

/*
 * Notification destructor.
 */
Notification::~Notification( void )
{
	// Notification destroyed.
}

/*
 * Initialize notification.
 */
bool Notification::initialize( const JUI::Texture *texture )
{
    // Stack for reporting.
    JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // Initialize container.
    if (!RoundedRectangleContainer::initialize()) {
        return false;
    }
	RoundedRectangle* rounded_rect = get_rounded_rectangle();
	rounded_rect->set_colour( &NOTIFICATION_COLOUR );
	rounded_rect->set_stroke( NOTIFICATION_STROKE_SIZE, &NOTIFICATION_STROKE_COLOUR );

    // Create layout.
    if (!JUTIL::BaseAllocator::allocate( &layout_ )) {
        return false;
    }
    layout_ = new (layout_) JUI::HorizontalLayout();
    if (!add( layout_ )) {
        JUTIL::BaseAllocator::destroy( layout_ );
        return false;
    }
    set_content( layout_ );

    // Create and add image if necessary.
    if (texture != nullptr) {
        JUI::Image* image;
        if (!JUTIL::BaseAllocator::allocate( &image )) {
            return false;
        }
        image = new (image) JUI::Image( texture );

        // Add to layout.
        if (!layout_->add( image )) {
            JUTIL::BaseAllocator::destroy( image );
            return false;
        }
        image->set_size( NOTIFICATION_ICON_SIZE, NOTIFICATION_ICON_SIZE );
    }

    // Add text.
    if (!JUTIL::BaseAllocator::allocate( &text_ )) {
        return false;
    }
    text_ = new (text_) JUI::WrappedText( font_, NOTIFICATION_TEXT_WIDTH );
	if (!layout_->add( text_ )) {
		JUTIL::BaseAllocator::destroy( text_ );
		return false;
	}
	text_->set_colour( &NOTIFICATION_FONT_COLOUR );
    pack();
    return true;
}

/*
 * Pack notification layout and parent class.
 */
void Notification::pack( void )
{
	// Pack layout, add padding.
	layout_->pack( NOTIFICATION_SPACING, JUI::ALIGN_MIDDLE );
	RoundedRectangleContainer::pack();
}

/*
 * Set text message.
 */
void Notification::set_message( const JUTIL::String* message )
{
	text_->set_text( message );
    pack();
}

/*
 * Precache notification-generic resources.
 */
bool Notification::precache( void )
{
	font_ = JUI::FontFactory::create_font( &NOTIFICATION_FONT_FACE, NOTIFICATION_FONT_SIZE );
    if (font_ == nullptr) {
        return false;
    }

    return true;
}