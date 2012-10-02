#include <jui/gfx/font_interface.hpp>
#include <jui/gfx/font_factory.hpp>
#include <jui/gfx/wrapped_text.hpp>
#include <jui/layout/horizontal_layout.hpp>

#include "notification.hpp"

const unsigned int NOTIFICATION_ICON_SIZE	= 48;
const unsigned int NOTIFICATION_TEXT_WIDTH	= 160;
const JUI::Colour NOTIFICATION_COLOUR( 25, 25, 25 );
const unsigned int NOTIFICATION_RADIUS		= 5;
const unsigned int NOTIFICATION_PADDING		= 10;
const unsigned int NOTIFICATION_SPACING		= 5;

const unsigned int NOTIFICATION_STROKE_SIZE	= 2;
const JUI::Colour NOTIFICATION_STROKE_COLOUR( 255, 255, 255, 100 );

const char* NOTIFICATION_FONT_FACE			= "fonts/tf2build.ttf";
const unsigned int NOTIFICATION_FONT_SIZE	= 10;
const JUI::Colour& NOTIFICATION_FONT_COLOUR = JUI::COLOUR_WHITE;

JUI::FontInterface *Notification::font_ = nullptr;

Notification::Notification( const JUTIL::ConstantString& message, const JUI::Texture *texture ) : RoundedRectangleContainer( NOTIFICATION_RADIUS, NOTIFICATION_PADDING )
{
	text_ = new JUI::WrappedText( font_, NOTIFICATION_TEXT_WIDTH );
	text_->set_colour( NOTIFICATION_FONT_COLOUR );

	layout_ = new JUI::HorizontalLayout();
	layout_->set_spacing( NOTIFICATION_SPACING );
	layout_->set_align_type( JUI::ALIGN_MIDDLE );
	add( layout_ );
	set_content( layout_ );

	// Only add image if there is one.
	image_ = nullptr;
	if (texture != nullptr) {
		image_ = new JUI::Image( texture );
		image_->set_size( NOTIFICATION_ICON_SIZE, NOTIFICATION_ICON_SIZE );
		layout_->add( image_ );
	}

	layout_->add( text_ );

	// Set rectangle attributes.
	RoundedRectangle* roundedRect = get_rounded_rectangle();
	roundedRect->set_colour( NOTIFICATION_COLOUR );
	roundedRect->set_stroke( NOTIFICATION_STROKE_SIZE, NOTIFICATION_STROKE_COLOUR );

	// Now set message, texture, and pack.
	set_message( message );
	pack();
}

Notification::~Notification( void )
{
	// Notification destroyed.
}

void Notification::pack( void )
{
	// Pack layout, add padding.
	layout_->pack();
	RoundedRectangleContainer::pack();
}

void Notification::set_message( const JUTIL::ConstantString& message )
{
	text_->set_text( message );
}

bool Notification::precache( void )
{
	font_ = JUI::FontFactory::create_font( NOTIFICATION_FONT_FACE, NOTIFICATION_FONT_SIZE );
    if (font_ == nullptr) {
        return false;
    }

    return true;
}

void Notification::release( void )
{
	if (font_ != nullptr) {
		delete font_;
		font_ = nullptr;
	}
}