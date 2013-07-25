#include <jui/gfx/font_interface.hpp>
#include <jui/gfx/font_factory.hpp>

#include "notice.hpp"

// Static members.
JUI::FontInterface* Notice::font_ = nullptr;

// Font constants.
const JUTIL::ConstantString NOTICE_FONT_FACE = "fonts/tf2build.ttf";
const unsigned int NOTICE_FONT_SIZE = 12;
const bool NOTICE_FONT_BOLDED = false;

// Notice specifications.
const unsigned int NOTICE_TEXT_WIDTH	= 300;

/*
 * Notice constructor.
 */
Notice::Notice( void )
{
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
    if (!Popup::initialize()) {
        return false;
    }

	// Add default text to layout.
    if (!JUTIL::BaseAllocator::allocate( &text_ )) {
        return false;
    }
	text_ = new (text_) JUI::WrappedText( font_, NOTICE_TEXT_WIDTH );
    if (!layout_->add( text_ )) {
        JUTIL::BaseAllocator::destroy( text_ );
        return false;
    }
    text_->set_text_formatting( DT_CENTER );

    // Set text message displayed by notice.
    set_message( message );
    return true;
}

/*
 * Set message to new string.
 * Pack must be called before notice is displayed after.
 */
void Notice::set_message( const JUTIL::String* message )
{
    // Update text object to show message.
	text_->set_text( message );
}

/*
 * Load notice-generic resources.
 */
bool Notice::precache( JUI::Graphics2D* graphics )
{
    // Allocate notice-generic font.
	font_ = JUI::FontFactory::create_font( &NOTICE_FONT_FACE, NOTICE_FONT_SIZE );
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