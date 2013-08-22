#include "button.hpp"

#include <jui/gfx/font_factory.hpp>
#include <jui/gfx/image.hpp>
#include <jui/gfx/text.hpp>

#include <memory/base_allocator.hpp>

JUI::FontInterface* Button::default_font_ = nullptr;

// Button icon size.
const unsigned int BUTTON_ICON_SIZE = 20;

// Button default font size.
const JUTIL::ConstantString BUTTON_DEFAULT_FONT_FACE = "fonts/tf2build.ttf";
const unsigned int BUTTON_DEFAULT_FONT_SIZE	= 14;
const JUI::Colour BUTTON_FONT_COLOUR( 42, 39, 37 );

// Rounded container parameters.
const unsigned int BUTTON_RADIUS		= 5;
const unsigned int BUTTON_SPACING		= 5;
const unsigned int BUTTON_PADDING		= 15;

// Button colours.
const JUI::Colour BUTTON_COLOUR( 247, 231, 198 );
const JUI::Colour BUTTON_COLOUR_HOVER( 180, 81, 14 );
const JUI::Colour BUTTON_COLOUR_DISABLED( 247, 231, 198, 100 );

/*
 * Button default constructor.
 */
Button::Button( void ) : RoundedRectangleContainer( BUTTON_PADDING )
{
}

/*
 * Button constructor by position.
 */
Button::Button( int x, int y ) : RoundedRectangleContainer( BUTTON_PADDING, x, y )
{
}

/*
 * Button layout management.
 */
bool Button::initialize( const JUTIL::String* label, const JUI::Texture* icon )
{
    // Initialize rounded container.
    if (!RoundedRectangleContainer::initialize()) {
        return false;
    }
    RoundedRectangle* rounded_rect = RoundedRectangleContainer::get_rounded_rectangle();
    rounded_rect->set_radius( BUTTON_RADIUS );

    // Generate layout.
    if (!JUTIL::BaseAllocator::allocate( &layout_ )) {
        return false;
    }
	layout_ = new (layout_) JUI::HorizontalLayout();
	if (!add( layout_ )) {
        JUTIL::BaseAllocator::destroy( layout_ );
        return false;
    }

    // Add label and icon if exist.
    if (icon != nullptr) {
        JUI::Image* image;
        if (!JUTIL::BaseAllocator::allocate( &image )) {
            return false;
        }
        new (image) JUI::Image( icon );
		image->set_size( icon->get_width(), icon->get_height() );
        if (!layout_->add( image )) {
            JUTIL::BaseAllocator::destroy( image );
            return false;
        }
    }
    if (label != nullptr) {
        JUI::Text* text;
        if (!JUTIL::BaseAllocator::allocate( &text )) {
            return false;
        }
        new (text) JUI::Text( default_font_ );
        if (!layout_->add( text )) {
            JUTIL::BaseAllocator::destroy( text );
            return false;
        }
        text->set_colour( &BUTTON_FONT_COLOUR );
	    text->set_text( label );
    }

	// Set rounded container attributes.
	set_content( layout_ );

	// Default attributes.
    set_enabled( true );
    set_hovering( false );
    update_button();
    pack();
    return true;
}

/*
 * Pack the button layout.
 */
void Button::pack( void )
{
	layout_->pack( BUTTON_SPACING, JUI::ALIGN_MIDDLE );
	RoundedRectangleContainer::pack();
}

/*
 * Update the button's colour based on hover state & enable mode.
 */
void Button::update_button( void )
{
	RoundedRectangle *rounded_rect = get_rounded_rectangle();
	const JUI::Colour* old_colour = rounded_rect->get_colour();
	const JUI::Colour* new_colour = nullptr;
	if ( is_enabled() ) {
        // Toggle hover-color if enabled.
        new_colour = (is_hovering_ ? &BUTTON_COLOUR_HOVER : &BUTTON_COLOUR);
	}
	else {
        // Same colour if disabled; hover not considered.
		new_colour = &BUTTON_COLOUR_DISABLED;
	}

	rounded_rect->set_colour( new_colour );
}

/*
 * Set button enable state.
 */
void Button::set_enabled( bool is_enabled )
{
	is_enabled_ = is_enabled;
	update_button();
}

/*
 * Check whether button is enabled.
 * Returns true if enabled, false otherwise.
 */
bool Button::is_enabled( void ) const
{
	return is_enabled_;
}

/*
 * Update hover state from mouse position.
 */
JUI::IOResult Button::on_mouse_moved( JUI::Mouse* mouse )
{
	// Return handled if hovering.
    bool colliding = mouse->is_touching( this );
    set_hovering( colliding );
    if (colliding) {
		return JUI::IO_RESULT_HANDLED;
	}

	return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Handle mouse click event.
 * Returns handled if enabled and mouse collides.
 */
JUI::IOResult Button::on_mouse_clicked( JUI::Mouse* mouse )
{
	// Handle if enabled and clicked.
	bool click_successful = is_enabled() && mouse->is_touching( this );
	if (click_successful) {
		return JUI::IO_RESULT_HANDLED;
	}
	
	return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Same behaviour as clicked.
 */
JUI::IOResult Button::on_mouse_released( JUI::Mouse* mouse )
{
	return on_mouse_clicked( mouse );
}

/*
 * Set button hover state.
 */
void Button::set_hovering( bool is_hovering )
{
	is_hovering_ = is_hovering;
    update_button();
}

/*
 * Return whether button is being hovered over by mouse.
 */
bool Button::is_hovering( void ) const
{
	return is_hovering_;
}

/*
 * Load button-generic resources.
 */
bool Button::precache( JUI::Graphics2D* graphics )
{
	default_font_ = JUI::FontFactory::create_font( &BUTTON_DEFAULT_FONT_FACE, BUTTON_DEFAULT_FONT_SIZE );
    if (default_font_ == nullptr) {
        return false;
    }
    return true;
}

/*
 * Release button-generic resources.
 */
void Button::release( void )
{
    JUI::FontFactory::destroy_font( default_font_ );
}