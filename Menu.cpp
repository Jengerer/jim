#include "Menu.h"

// Menu created.
Menu::Menu( void )
{
	SetClicked( nullptr );
	SetState( POPUP_STATE_HIDDEN );

	// Create rounded rectangle.
	roundedRect_ = new RoundedRectangle( 0, 0, MENU_RADIUS, MENU_BACKGROUND_COLOUR );
	roundedRect_->SetStroke( MENU_STROKE_SIZE, MENU_STROKE_COLOUR );
	roundedRect_->SetLocalPosition( 0, 0 );
	Add( roundedRect_ );

	// Create layout.
	layout_ = new VerticalLayout();
	layout_->SetLocalPosition( MENU_PADDING, MENU_PADDING );
	Add( layout_ );
}

Menu::~Menu( void )
{
	// Menu destroyed.
}

LabelButton* Menu::AddOption( const string& caption, Texture *texture )
{
	LabelButton *button = new LabelButton( caption, texture );
	button->SetIcon( texture );

	// Add to layout and list.
	options_.push_back( button );
	layout_->Add( button );

	return button;
}

LabelButton* Menu::GetClicked( void ) const
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

bool Menu::OnMouseMoved( Mouse *mouse )
{
	// Pass message to buttons.
	vector<LabelButton*>::iterator i;
	for (i = options_.begin(); i != options_.end(); i++) {
		Button *button = *i;
		button->OnMouseMoved( mouse );
	}

	return true;
}

bool Menu::OnLeftClicked( Mouse *mouse )
{
	SetClicked( nullptr );
	if ( !mouse->IsTouching( this ) ) {
		SetState( POPUP_STATE_HIDDEN );
	}

	// Menu has priority over all under it.
	return true;
}

bool Menu::OnLeftReleased( Mouse *mouse )
{
	if (mouse->IsTouching( this )) {
		vector<LabelButton*>::iterator i;
		for (i = options_.begin(); i != options_.end(); i++) {
			LabelButton *button = *i;

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

void Menu::SetClicked( LabelButton *clicked )
{
	clicked_ = clicked;
}