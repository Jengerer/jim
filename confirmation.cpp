#include "confirmation.hpp"
#include <jui/layout/horizontal_layout.hpp>
#include <string/string.hpp>

const int BUTTON_SPACING = 10;

// Button strings.
const JUTIL::ConstantString YES_BUTTON_LABEL = "yes";
const JUTIL::ConstantString NO_BUTTON_LABEL = "no";

/*
 * Confirmation constructor.
 */
Confirmation::Confirmation( void )
    : response_( RESPONSE_NULL )
{
}

/*
 * Confirmation destructor.
 */
Confirmation::~Confirmation( void )
{
}

/*
 * Generate layout for confirmation.
 */
bool Confirmation::initialize( const JUTIL::String* question )
{
    // Base button notice initialization.
    if (!ButtonNotice::initialize( question )) {
        return false;
    }

    // Create layout for buttons.
	JUI::HorizontalLayout* buttons;
    if (!JUTIL::BaseAllocator::allocate( &buttons )) {
        return false;
    }
    new (buttons) JUI::HorizontalLayout();
    if (!layout_->add( buttons )) {
        JUTIL::BaseAllocator::destroy( buttons );
        return false;
    }

    // Create response buttons.
    yes_ = button_manager_.create( &YES_BUTTON_LABEL, nullptr );
    if (yes_ == nullptr) {
        return false;
    }
    if (!buttons->add( yes_ )) {
        JUTIL::BaseAllocator::destroy( yes_ );
        return false;
    }
	no_ = button_manager_.create( &NO_BUTTON_LABEL, nullptr );
    if (no_ == nullptr) {
        return false;
    }
    if (!buttons->add( no_ )) {
        JUTIL::BaseAllocator::destroy( no_ );
        return false;
    }

    return true;
}

/*
 * Get the response that the user selected, or RESPONSE_NULL
 * if no response made yet.
 */
ConfirmationResponse Confirmation::get_response( void ) const
{
	return response_;
}

/*
 * Handle button release event.
 */
bool Confirmation::on_button_released( Button* button )
{
    assert( button == yes_button_ || button == no_button_ );
    if (button == yes_button_) {
        response_ = RESPONSE_YES;
    }
    else {
        response_ = RESPONSE_NO;
    }
    set_state( POPUP_STATE_KILLED );
}