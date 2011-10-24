#include "confirmation.h"

const int BUTTON_SPACING = 10;

Confirmation::Confirmation( const std::string& question ) : Notice( question )
{
	// Create response buttons.
	yes_ = Button::CreateLabelButton( "yes" );
	no_ = Button::CreateLabelButton( "no" );
	response_ = RESPONSE_NULL;
	
	// Organize in layout and add.
	HorizontalLayout* layout = new HorizontalLayout( BUTTON_SPACING );
	layout->add( yes_ );
	layout->add( no_ );
	layout->pack();
	content_->add( layout );

	// Pack it up!
	pack();
}

Confirmation::~Confirmation( void )
{
	// Confirmation deleted.
}

ConfirmationResponse Confirmation::get_response( void ) const
{
	return response_;
}

bool Confirmation::on_mouse_released( Mouse* mouse )
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

bool Confirmation::on_mouse_moved( Mouse* mouse )
{
	// Notify buttons.
	yes_->on_mouse_moved( mouse );
	no_->on_mouse_moved( mouse );

	return Notice::on_mouse_moved( mouse );
}