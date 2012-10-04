#include "alert.hpp"

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
bool Alert::initialize( const JUTIL::ConstantString& message )
{
    // Create notice.
    if (!Notice::initialize( message ))
    {
        return false;
    }

    // Make OK button.
    ok_ = Button::create_label_button( "okay" );
    if (ok_ == nullptr) {
        return false;
    }
    if (!content_->add( ok_ ))
    {
        JUTIL::BaseAllocator::destroy( ok_ );
        return false;
    }
    pack();
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
bool Alert::on_key_released( int key )
{
	if (key == VK_RETURN) {
		set_state( POPUP_STATE_KILLED );
	}

	return true;
}

/*
 * Hover over button.
 */
bool Alert::on_mouse_moved( JUI::Mouse* mouse )
{
	// Parent behaviour.
	ok_->on_mouse_moved( mouse );
	return Notice::on_mouse_moved( mouse );
}

/*
 * Click on button.
 */
bool Alert::on_mouse_clicked( JUI::Mouse* mouse )
{
	return Notice::on_mouse_clicked( mouse );
}

/*
 * Release on button.
 */
bool Alert::on_mouse_released( JUI::Mouse* mouse )
{
    // Set to killed if okay pressed.
	if (mouse->is_touching( this )) {
		if (ok_->on_mouse_released( mouse )) {
			set_state( POPUP_STATE_KILLED );
		}

		return true;
	}

	return false;
}