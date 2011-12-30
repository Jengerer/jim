#include <jui/ifont.h>
#include <jui/font_factory.h>

#include "notification.h"

const unsigned int NOTIFICATION_ICON_SIZE	= 48;
const unsigned int NOTIFICATION_TEXT_WIDTH	= 160;
const Colour NOTIFICATION_COLOUR( 25, 25, 25 );
const unsigned int NOTIFICATION_RADIUS		= 5;
const unsigned int NOTIFICATION_PADDING		= 10;
const unsigned int NOTIFICATION_SPACING		= 5;

const unsigned int NOTIFICATION_STROKE_SIZE	= 2;
const Colour NOTIFICATION_STROKE_COLOUR( 255, 255, 255, 100 );

const char* NOTIFICATION_FONT_FACE			= "fonts/tf2build.ttf";
const unsigned int NOTIFICATION_FONT_SIZE	= 10;
const Colour& NOTIFICATION_FONT_COLOUR		= COLOUR_WHITE;

IFont *Notification::font_ = nullptr;

Notification::Notification( const std::string& message, const Texture *texture ) : RoundedRectangleContainer( NOTIFICATION_RADIUS, NOTIFICATION_PADDING )
{
	text_ = new WrappedText( font_, NOTIFICATION_TEXT_WIDTH );
	text_->set_colour( NOTIFICATION_FONT_COLOUR );

	layout_ = new HorizontalLayout();
	layout_->set_spacing( NOTIFICATION_SPACING );
	layout_->set_align_type( ALIGN_MIDDLE );
	add( layout_ );
	SetContent( layout_ );

	// Only add image if there is one.
	image_ = nullptr;
	if (texture != nullptr) {
		image_ = new Image( texture );
		image_->set_size( NOTIFICATION_ICON_SIZE, NOTIFICATION_ICON_SIZE );
		layout_->add( image_ );
	}

	layout_->add( text_ );

	// Set rectangle attributes.
	RoundedRectangle* roundedRect = GetRoundedRectangle();
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

void Notification::set_message( const std::string& message )
{
	text_->set_text( message );
}

void Notification::precache()
{
	font_ = FontFactory::create_font( NOTIFICATION_FONT_FACE, NOTIFICATION_FONT_SIZE );
}

void Notification::release( void )
{
	if (font_ != nullptr) {
		delete font_;
		font_ = nullptr;
	}
}