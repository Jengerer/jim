#include <jui/ifont.h>
#include <jui/font_factory.h>

#include "notice.h"

// Static members.
IFont* Notice::font_						= nullptr;

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
const Colour NOTICE_STROKE_COLOUR( 255, 255, 255, 100 );
const Colour& NOTICE_COLOUR				= COLOUR_BLACK;

Notice::Notice( const std::string& message ) : Popup( 0.0f, 0.0f )
{
	// Create rounded container.
	roundedContainer_ = new RoundedRectangleContainer( NOTICE_RADIUS, NOTICE_PADDING );
	RoundedRectangle *roundedRect = roundedContainer_->get_rounded_rectangle();
	roundedRect->set_stroke( NOTICE_STROKE_WIDTH, NOTICE_STROKE_COLOUR );
	roundedRect->set_stroke_type( STROKE_TYPE_OUTER );
	roundedRect->set_colour( NOTICE_COLOUR );
	add( roundedContainer_ );
	set_constraint( roundedContainer_, 0.0f, 0.0f );

	// Create layout for container.
	content_ = new VerticalLayout();
	content_->set_spacing( NOTICE_SPACING );
	roundedContainer_->add( content_ );
	roundedContainer_->set_content( content_ );

	// Add default text to layout.
	text_ = new WrappedText( font_, NOTICE_TEXT_WIDTH );
	text_->set_text_formatting( DT_CENTER );
	content_->add( text_ );
	set_message( message );
	pack();
}

Notice::~Notice()
{
	// Notice is destroyed.
}

void Notice::pack( void )
{
	content_->pack();
	roundedContainer_->pack();
	set_size( roundedContainer_->get_width(), roundedContainer_->get_height() );
}

void Notice::set_message( const std::string& message )
{
	message_ = message;
	text_->set_text( message );
	pack();
}

void Notice::append_message( const std::string& message )
{
	set_message( message_ + message );
}

void Notice::precache( Graphics2D* graphics )
{
	font_ = FontFactory::create_font( NOTICE_FONT_FACE, NOTICE_FONT_SIZE );
}

void Notice::release( void )
{
	if (font_ != nullptr) {
		delete font_;
		font_ = nullptr;
	}
}