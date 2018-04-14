#include "confirmation.hpp"
#include <jui/layout/horizontal_layout.hpp>
#include <string/string.hpp>
#include <cassert>

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
    yes_button_ = button_manager_.create( &YES_BUTTON_LABEL, nullptr );
    if (yes_button_ == nullptr) {
        return false;
    }
    if (!buttons->add( yes_button_ )) {
        JUTIL::BaseAllocator::destroy( yes_button_ );
        return false;
    }
	no_button_ = button_manager_.create( &NO_BUTTON_LABEL, nullptr );
    if (no_button_ == nullptr) {
        return false;
    }
    if (!buttons->add( no_button_ )) {
        JUTIL::BaseAllocator::destroy( no_button_ );
        return false;
    }
	buttons->pack( BUTTON_SPACING, JUI::ALIGN_MIDDLE );

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
    return true;
}