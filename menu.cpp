#include "menu.h"

const unsigned int MENU_PADDING		= 10;
const unsigned int MENU_SPACING		= 10;
const unsigned int MENU_RADIUS		= 3;
const unsigned int MENU_STROKE_SIZE	= 4;
const Colour& MENU_STROKE_COLOUR	= COLOUR_WHITE;
const Colour MENU_BACKGROUND_COLOUR( 42, 39, 37 );

// Menu created.
Menu::Menu( void )
{
	set_clicked( nullptr );
	set_state( POPUP_STATE_HIDDEN );

	// Create rounded rectangle.
	rounded_rect_ = new RoundedRectangle( 0, 0, MENU_RADIUS, MENU_BACKGROUND_COLOUR );
	rounded_rect_->set_stroke( MENU_STROKE_SIZE, MENU_STROKE_COLOUR );
	set_constraint( rounded_rect_, 0.0f, 0.0f );
	add( rounded_rect_ );

	// Create layout.
	layout_ = new VerticalLayout();
	float constraint_padding = static_cast<float>(MENU_PADDING);
	set_constraint( layout_, constraint_padding, constraint_padding );
	add( layout_ );
}

Menu::~Menu( void )
{
	// Menu destroyed.
}

Button* Menu::AddOption( const std::string& caption )
{
	Button* button = Button::create_label_button( caption );

	// Add to layout and list.
	options_.push_back( button );
	layout_->add( button );

	return button;
}

Button* Menu::GetClicked( void ) const
{
	return clicked_;
}

void Menu::pack( void )
{
	layout_->pack();
	rounded_rect_->set_size( layout_->get_width() + MENU_PADDING * 2,
		layout_->get_height() + MENU_PADDING * 2 );
	set_size( rounded_rect_->get_width(), rounded_rect_->get_height() );
}

bool Menu::on_mouse_moved( Mouse *mouse )
{
	// Pass message to buttons.
	vector<Button*>::iterator i, end;
	for (i = options_.begin(), end = options_.end(); i != end; ++i) {
		Button *button = *i;
		button->on_mouse_moved( mouse );
	}

	return true;
}

bool Menu::on_mouse_clicked( Mouse *mouse )
{
	set_clicked( nullptr );
	if ( !mouse->is_touching( this ) ) {
		set_state( POPUP_STATE_HIDDEN );
	}

	// Menu has priority over all under it.
	return true;
}

bool Menu::on_mouse_released( Mouse *mouse )
{
	if (mouse->is_touching( this )) {
		vector<Button*>::iterator i;
		for (i = options_.begin(); i != options_.end(); i++) {
			Button *button = *i;

			// Set selected button, and go inactive.
			if (mouse->is_touching( button )) {
				set_state( POPUP_STATE_HIDDEN );
				set_clicked( button );
				break;
			}
		}

		return true;
	}

	return false;
}

void Menu::set_clicked( Button *clicked )
{
	clicked_ = clicked;
}