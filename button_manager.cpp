#include "button_manager.hpp"

ButtonManager::ButtonManager( void ) :
	clicked_( nullptr ),
	buttons_(),
	listener_( nullptr )
{
}

ButtonManager::~ButtonManager( void )
{
}

/*
 * Set the interface pointer for the button event listener.
 * This pointer must be set before button events are handled.
 */
void ButtonManager::set_event_listener( ButtonListener* listener )
{
    listener_ = listener;
}

/*
 * Create and return a pointer to the new button.
 * Adds to the button manager after creation.
 */
Button* ButtonManager::create( const JUTIL::String* label, const JUI::Texture* icon )
{
    // Allocate new button.
    Button* button;
    if (!JUTIL::BaseAllocator::allocate( &button )) {
        return nullptr;
    }
    new (button) Button();
    if (!button->initialize( label, icon )) {
        JUTIL::BaseAllocator::destroy( button );
        return nullptr;
    }

    // Add button to vector.
    if (!add( button )) {
        JUTIL::BaseAllocator::destroy( button );
        return nullptr;
    }
    return button;
}

/*
 * Add the button to be managed by the button manager.
 */
bool ButtonManager::add( Button* button )
{
    if (!buttons_.push( button )) {
        return false;
    }
    return true;
}

/*
 * Remove button from manager (does not destroy the button).
 */
bool ButtonManager::remove( Button* button )
{
    // Remove from array.
    if (!buttons_.remove( button )) {
        return false;
    }
    
    // Reset clicked button if removed.
    if (clicked_ == button) {
        clicked_ = nullptr;
    }

    return true;
}

/*
 * Handle button hover events on mouse move.
 */
JUI::IOResult ButtonManager::on_mouse_moved( JUI::Mouse* mouse )
{
    unsigned int i;
    unsigned int end = buttons_.get_length();
    for (i = 0; i < end; ++i) {
        Button* button = buttons_.at( i );
        button->on_mouse_moved( mouse );
    }

    return JUI::IO_RESULT_HANDLED;
}

/*
 * Handles button press events assuming only one button is clicked at a time.
 */
JUI::IOResult ButtonManager::on_mouse_clicked( JUI::Mouse* mouse )
{
    unsigned int i;
    unsigned int end = buttons_.get_length();
    for (i = 0; i < end; ++i) {
        Button* button = buttons_.at( i );
        JUI::IOResult result = button->on_mouse_clicked( mouse );
        if (result == JUI::IO_RESULT_HANDLED) {
            if (!listener_->on_button_pressed( button )) {
                return JUI::IO_RESULT_ERROR;
            }

            clicked_ = button;
            return JUI::IO_RESULT_HANDLED;
        }
    }

    clicked_ = nullptr;
    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Handle button release events assuming button has been clicked before releasing.
 */
JUI::IOResult ButtonManager::on_mouse_released( JUI::Mouse* mouse )
{
    // Check clicked button.
    if (clicked_ != nullptr) {
        JUI::IOResult result = clicked_->on_mouse_released( mouse );
        if (result == JUI::IO_RESULT_HANDLED) {
            if (!listener_->on_button_released( clicked_ )) {
                return JUI::IO_RESULT_ERROR;
            }

            clicked_ = nullptr;
            return JUI::IO_RESULT_HANDLED;
        }
    }

    return JUI::IO_RESULT_UNHANDLED;
}