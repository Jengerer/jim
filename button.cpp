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
bool Button::initialize( void )
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
	layout_ = new (layout_) JUI::HorizontalLayout( BUTTON_SPACING, JUI::ALIGN_MIDDLE );
	if (!add( layout_ )) {
        JUTIL::BaseAllocator::destroy( layout_ );
        return false;
    }

	// Set rounded container attributes.
	set_content( layout_ );

	// Default attributes.
    set_enabled( true );
    set_hovering( false );
    update_button();
    return true;
}

/*
 * Pack the button layout.
 */
void Button::pack( void )
{
	layout_->pack();
	RoundedRectangleContainer::pack();
}

/*
 * Get a handle to the button's content layout object.
 */
JUI::Layout* Button::get_content_layout( void ) const
{
	return layout_;
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

/*
 * Create generic button.
 */
Button* Button::create_generic_button( void )
{
	// Create and initialize button.
	Button* button;
	if (!JUTIL::BaseAllocator::allocate( &button )) {
		return nullptr;
	}
	button = new (button) Button();
	if (!button->initialize()) {
		JUTIL::BaseAllocator::destroy( button );
		return nullptr;
	}

	return button;
}

/*
 * Create icon button given a texture.
 */
Button* Button::create_icon_button( JUI::Texture* texture )
{
    // Start with generic button.
    Button* button = create_generic_button();
    if (button == nullptr) {
        return nullptr;
    }
	JUI::Layout* layout = button->get_content_layout();

    // Create image.
    JUI::Image* icon;
    if (!JUTIL::BaseAllocator::allocate( &icon )) {
        JUTIL::BaseAllocator::destroy( button );
        return nullptr;
    }
    icon = new (icon) JUI::Image( texture );
	if (!layout->add( icon ))
    {
        JUTIL::BaseAllocator::destroy( icon );
        JUTIL::BaseAllocator::destroy( button );
        return nullptr;
    }

    // Pack and return.
	button->pack();
	return button;
}

/*
 * Create a button with a label.
 */
Button* Button::create_label_button( const JUTIL::String* label, JUI::FontInterface* font )
{
    // Start with generic button.
    Button* button = create_generic_button();
    if (button == nullptr) {
        return nullptr;
    }
	JUI::Layout* layout = button->get_content_layout();

    // Create label.
    JUI::Text *text;
    if (!JUTIL::BaseAllocator::allocate( &text )) {
        JUTIL::BaseAllocator::destroy( button );
        return nullptr;
    }
    text = new (text) JUI::Text( font );
	text->set_colour( &BUTTON_FONT_COLOUR );
	text->set_text( label );
	if (!layout->add( text ))
    {
        JUTIL::BaseAllocator::destroy( text );
        JUTIL::BaseAllocator::destroy( button );
        return nullptr;
    }

    // Pack and return.
	button->pack();
	return button;
}

/*
 * Create button with both icon and label.
 */
Button* Button::create_icon_label_button( JUI::Texture* texture, const JUTIL::String* label, JUI::FontInterface* font )
{
    // Start with generic button.
    Button* button = create_generic_button();
    if (button == nullptr) {
        return nullptr;
    }
	JUI::Layout* layout = button->get_content_layout();

    // Create image.
    JUI::Image* icon;
    if (!JUTIL::BaseAllocator::allocate( &icon )) {
        JUTIL::BaseAllocator::destroy( button );
        return nullptr;
    }
    icon = new (icon) JUI::Image( texture );
	if (!layout->add( icon )) {
        JUTIL::BaseAllocator::destroy( icon );
        JUTIL::BaseAllocator::destroy( button );
        return nullptr;
    }
    icon->set_size( BUTTON_ICON_SIZE, BUTTON_ICON_SIZE );

    // Create label.
    JUI::Text *text;
    if (!JUTIL::BaseAllocator::allocate( &text )) {
        JUTIL::BaseAllocator::destroy( button );
        return nullptr;
    }
    text = new (text) JUI::Text( font );
	text->set_colour( &BUTTON_FONT_COLOUR );
	text->set_text( label );
	if (!layout->add( text )) {
        JUTIL::BaseAllocator::destroy( text );
        JUTIL::BaseAllocator::destroy( button );
        return nullptr;
    }

    // Pack and return.
	button->pack();
	return button;
}