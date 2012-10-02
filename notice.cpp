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
 * Notice creation function for safe allocation.
 */
Notice* Notice::create( const JUTIL::ConstantString& message )
{
    // Create rounded container.
	RoundedRectangleContainer* rounded_container = new RoundedRectangleContainer( NOTICE_RADIUS, NOTICE_PADDING );
	RoundedRectangle* rounded_rect = rounded_container->get_rounded_rectangle();
	rounded_rect->set_stroke( NOTICE_STROKE_WIDTH, NOTICE_STROKE_COLOUR );
	rounded_rect->set_stroke_type( STROKE_TYPE_OUTER );
	rounded_rect->set_colour( NOTICE_COLOUR );
	add( rounded_container_ );
	set_constraint( rounded_container_, 0.0f, 0.0f );

	// Create layout for container.
	content_ = new JUI::VerticalLayout();
	content_->set_spacing( NOTICE_SPACING );
	rounded_container_->add( content_ );
	rounded_container_->set_content( content_ );

	// Add default text to layout.
	text_ = new JUI::WrappedText( font_, NOTICE_TEXT_WIDTH );
	text_->set_text_formatting( DT_CENTER );
	content_->add( text_ );
	set_message( message );
}

Notice::Notice( RoundedRectangleContainer* rounded_container, JUI::WrappedText* text, JUTIL::StringBuilder* builder )
    : rounded_container_( rounded_container ),
    text_( text ),
    message_( builder )
{
	// Nothing speicla.
}

Notice::~Notice()
{
	// Notice is destroyed.
}

void Notice::pack( void )
{
	content_->pack();
	rounded_container_->pack();
	set_size( rounded_container_->get_width(), rounded_container_->get_height() );
}

void Notice::set_message( const JUTIL::ConstantString& message )
{
	message_ = message;
	text_->set_text( message );
	pack();
}

void Notice::append_message( const std::string& message )
{
	set_message( message_ + message );
}

void Notice::precache( JUI::Graphics2D* graphics )
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