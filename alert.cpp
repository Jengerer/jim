#include "alert.hpp"

// Button label strings.
const JUTIL::ConstantString OKAY_BUTTON_LABEL = "okay";

/*
 * Construct alert message.
 */
Alert::Alert( void )
{
}

/*
 * Alert message destructor.
 */
Alert::~Alert( void )
{
}

/*
 * Add button to layout on creation.
 */
bool Alert::initialize( const JUTIL::String* message )
{
    // Create notice.
    if (!Notice::initialize( message ))
    {
        return false;
    }

    // Make OK button.
    ok_ = Button::create_label_button( &OKAY_BUTTON_LABEL );
    if (ok_ == nullptr) {
        return false;
    }
    if (!content_->add( ok_ ))
    {
        JUTIL::BaseAllocator::destroy( ok_ );
        return false;
    }
    pack();
    return true;
}

/*
 * Get handle to alert button.
 */
const Button* Alert::get_button( void ) const
{
	return ok_;
}

/*
 * Check whether popup was closed by key.
 */
JUI::IOResult Alert::on_key_released( int key )
{
	// Handled if enter pressed.
	if (key == VK_RETURN) {
		set_state( POPUP_STATE_KILLED );
		return JUI::IO_RESULT_HANDLED;
	}

	return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Hover over button.
 */
JUI::IOResult Alert::on_mouse_moved( JUI::Mouse* mouse )
{
	// Parent behaviour.
	JUI::IOResult result = ok_->on_mouse_moved( mouse );
	if (result != JUI::IO_RESULT_UNHANDLED) {
		return result;
	}

	return Notice::on_mouse_moved( mouse );
}

/*
 * Click on button.
 */
JUI::IOResult Alert::on_mouse_clicked( JUI::Mouse* mouse )
{
	return Notice::on_mouse_clicked( mouse );
}

/*
 * Release on button.
 */
JUI::IOResult Alert::on_mouse_released( JUI::Mouse* mouse )
{
    // Set to killed if okay pressed.
	JUI::IOResult result = ok_->on_mouse_released( mouse );
	if (result == JUI::IO_RESULT_HANDLED) {
		set_state( POPUP_STATE_KILLED );
	}

	return JUI::IO_RESULT_HANDLED;
}