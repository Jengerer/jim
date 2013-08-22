#include "popup.hpp"

// Popup UI constants.
const unsigned int POPUP_PADDING = 20;
const unsigned int POPUP_CORNER_RADIUS = 5;
const unsigned int POPUP_STROKE_WIDTH = 5;
const unsigned int POPUP_CONTENT_SPACING = 10;
const JUI::Colour POPUP_STROKE_COLOUR( 255, 255, 255, 100 );
const JUI::Colour* POPUP_COLOUR = &JUI::COLOUR_BLACK;

/*
 * Default popup constructor.
 */
Popup::Popup( void ) : RoundedRectangleContainer( POPUP_PADDING )
{
}

/*
 * Popup constructor with default position.
 */
Popup::Popup( int x, int y ) : RoundedRectangleContainer( POPUP_PADDING, x, y )
{
}

/*
 * Popup destructor.
 */
Popup::~Popup( void )
{
	// Popup is removed.
}

/*
 * Popup initialization.
 * No pack should be done in this or derived initialization methods.
 */
bool Popup::initialize( void )
{
    // Initialize rounded container.
    if (!RoundedRectangleContainer::initialize()) {
        return false;
    }

    // Create layout to store in rounded rectangle.
    if (!JUTIL::BaseAllocator::allocate( &layout_ )) {
        return false;
    }
    new (layout_) JUI::VerticalLayout();
    if (!add( layout_ )) {
        JUTIL::BaseAllocator::destroy( layout_ );
        return false;
    }
        
    // Style rounded rectangle.
    RoundedRectangle* rounded_rect = get_rounded_rectangle();
    rounded_rect->set_radius( POPUP_CORNER_RADIUS );
	rounded_rect->set_stroke( POPUP_STROKE_WIDTH, &POPUP_STROKE_COLOUR );
	rounded_rect->set_stroke_type( STROKE_TYPE_OUTER );
	rounded_rect->set_colour( POPUP_COLOUR );
	return true;
}

/*
 * Pack the popup layout.
 */
void Popup::pack( void )
{
    layout_->pack( POPUP_CONTENT_SPACING, JUI::ALIGN_CENTER );
    RoundedRectangleContainer::pack();
}

/*
 * Set popup state.
 */
void Popup::set_state( PopupState state )
{
	state_ = state;
}

/*
 * Get popup state.
 */
PopupState Popup::get_state( void ) const
{
	return state_;
}

/*
 * Handle mouse clicks on popup.
 * Popups always take precedence, so if we get event, return handled.
 */
JUI::IOResult Popup::on_mouse_clicked( JUI::Mouse* mouse )
{
    // Return handled if mouse is touching.
    return JUI::IO_RESULT_HANDLED;
}

/*
 * Handle mouse releases on popup.
 * Popups always take precedence, so if we get event, return handled.
 */
JUI::IOResult Popup::on_mouse_released( JUI::Mouse* mouse )
{
    return JUI::IO_RESULT_HANDLED;
}

/*
 * Handle mouse movement.
 * Popups always take precedence, so if we get event, return handled.
 */
JUI::IOResult Popup::on_mouse_moved( JUI::Mouse* mouse )
{
	// Return handled if mouse is touching.
    return JUI::IO_RESULT_HANDLED;
}

/*
 * Handle key press events.
 * Popups always take precedence, so if we get event, return handled.
 */
JUI::IOResult Popup::on_key_pressed( int key )
{
    return JUI::IO_RESULT_HANDLED;
}

/*
 * Handle key release events.
 * Popups always take precedence, so if we get event, return handled.
 */
JUI::IOResult Popup::on_key_released( int key )
{
	// Generic popup doesn't handle keys.
    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Position popup to center of container.
 */
void Popup::center_to( const Container* parent )
{
    int mid_x = parent->get_x() + (parent->get_width() - get_width()) / 2;
    int mid_y = parent->get_y() + (parent->get_height() - get_height()) / 2;
    set_position( mid_x, mid_y );
}