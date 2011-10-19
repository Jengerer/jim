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
	SetClicked( nullptr );
	SetState( POPUP_STATE_HIDDEN );

	// Create rounded rectangle.
	roundedRect_ = new RoundedRectangle( 0, 0, MENU_RADIUS, MENU_BACKGROUND_COLOUR );
	roundedRect_->SetStroke( MENU_STROKE_SIZE, MENU_STROKE_COLOUR );
	SetConstraint( roundedRect_, 0.0f, 0.0f );
	Add( roundedRect_ );

	// Create layout.
	layout_ = new VerticalLayout();
	SetConstraint( layout_, MENU_PADDING, MENU_PADDING );
	Add( layout_ );
}

Menu::~Menu( void )
{
	// Menu destroyed.
}

Button* Menu::AddOption( const std::string& caption )
{
	Button* button = Button::CreateLabelButton( caption );

	// Add to layout and list.
	options_.push_back( button );
	layout_->Add( button );

	return button;
}

Button* Menu::GetClicked( void ) const
{
	return clicked_;
}

void Menu::Pack( void )
{
	layout_->Pack();
	roundedRect_->SetSize( layout_->GetWidth() + MENU_PADDING * 2,
		layout_->GetHeight() + MENU_PADDING * 2 );
	SetSize( roundedRect_->GetWidth(), roundedRect_->GetHeight() );
}

bool Menu::MouseMoved( Mouse *mouse )
{
	// Pass message to buttons.
	vector<Button*>::iterator i, end;
	for (i = options_.begin(), end = options_.end(); i != end; ++i) {
		Button *button = *i;
		button->MouseMoved( mouse );
	}

	return true;
}

bool Menu::MouseClicked( Mouse *mouse )
{
	SetClicked( nullptr );
	if ( !mouse->IsTouching( this ) ) {
		SetState( POPUP_STATE_HIDDEN );
	}

	// Menu has priority over all under it.
	return true;
}

bool Menu::MouseReleased( Mouse *mouse )
{
	if (mouse->IsTouching( this )) {
		vector<Button*>::iterator i;
		for (i = options_.begin(); i != options_.end(); i++) {
			Button *button = *i;

			// Set selected button, and go inactive.
			if (mouse->IsTouching( button )) {
				SetState( POPUP_STATE_HIDDEN );
				SetClicked( button );
				break;
			}
		}

		return true;
	}

	return false;
}

void Menu::SetClicked( Button *clicked )
{
	clicked_ = clicked;
}