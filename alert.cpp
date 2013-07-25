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
    if (!ButtonNotice::initialize( message )) {
        return false;
    }

    // Make OK button.
    ok_button_ = button_manager_.create( &OKAY_BUTTON_LABEL, nullptr );
    if (ok_button_ == nullptr) {
        return false;
    }
    if (!layout_->add( ok_button_ )) {
        JUTIL::BaseAllocator::destroy( ok_button_ );
        return false;
    }
    return true;
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
 * Handle OK button release event to change alert state.
 */
bool Alert::on_button_released( Button* button )
{
    // We expect that the button pressed is the only one; OK...
    assert( button == ok_button_ );
    set_state( POPUP_STATE_KILLED );
    return true;
}
