#include "confirmation.hpp"
#include <jui/layout/horizontal_layout.hpp>

const int BUTTON_SPACING = 10;

/*
 * Confirmation constructor.
 */
Confirmation::Confirmation( void )
{
    response_ = RESPONSE_NULL;
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
    // Base notice initialization.
    if (!Notice::initialize( question ))
    {
        return false;
    }

    // Create response buttons.
	yes_ = Button::create_label_button( "yes" );
    if (yes_ == nullptr) {
        return false;
    }

	no_ = Button::create_label_button( "no" );
    if (no_ == nullptr) {
        JUTIL::BaseAllocator::destroy( yes_ );
        return false;
    }
	
	// Create layout for buttons.
	JUI::HorizontalLayout* layout;
    if (!JUTIL::BaseAllocator::allocate( &layout )) {
        JUTIL::BaseAllocator::destroy( yes_ );
        JUTIL::BaseAllocator::destroy( no_ );
        return false;
    }
    layout = new (layout) JUI::HorizontalLayout( BUTTON_SPACING );
    if (!content_->add( layout )) {
        JUTIL::BaseAllocator::destroy( yes_ );
        JUTIL::BaseAllocator::destroy( no_ );
        JUTIL::BaseAllocator::destroy( layout );
        return false;
    }

    // Add buttons to layout.
	if (!layout->add( yes_ )) {
        JUTIL::BaseAllocator::destroy( yes_ );
        JUTIL::BaseAllocator::destroy( no_ );
        return false;
    }
    else if (!layout->add( no_ )) {
        JUTIL::BaseAllocator::destroy( no_ );
        return false;
    }

	// Pack it up!
    layout->pack();
    pack();
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
 * Trigger response based on which button pressed.
 */
bool Confirmation::on_mouse_released( JUI::Mouse* mouse )
{
	// Set response based on button clicked.
	if (yes_->on_mouse_released( mouse )) {
		response_ = RESPONSE_YES;
	}
	else if (no_->on_mouse_released( mouse )) {
		response_ = RESPONSE_NO;
	}
	else {
		return false;
	}

	// Responded.
	set_state( POPUP_STATE_KILLED );
	return true;
}

/*
 * Hover over response buttons.
 */
bool Confirmation::on_mouse_moved( JUI::Mouse* mouse )
{
	// Notify buttons.
	yes_->on_mouse_moved( mouse );
	no_->on_mouse_moved( mouse );
	return Notice::on_mouse_moved( mouse );
}