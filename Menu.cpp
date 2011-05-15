#include "Menu.h"

// Menu created.
Menu::Menu()
{
	selected_	= 0;
	widest_		= 0;
	setState( POPUP_STATE_INACTIVE );
}

Menu::~Menu()
{
	// Menu destroyed.
}

void Menu::draw( DirectX *directX )
{
	// Draw stroked rounded rectangle.
	directX->DrawRoundedRect( GetX(), GetY(), GetWidth(), GetHeight(), MENU_RADIUS + MENU_STROKE, MENU_STROKE_COLOUR );
	directX->DrawRoundedRect( GetX() + MENU_STROKE, GetY() + MENU_STROKE, GetWidth() - MENU_STROKE * 2, GetHeight() - MENU_STROKE * 2, MENU_RADIUS, MENU_BACKGROUND_COLOUR );

	// Draw buttons.
	for (int i = 0; i < options_.size(); i++) {
		Option *button = options_[i];
		button->OnDraw( directX );
	}
}

Option* Menu::addOption( const string& caption, Texture *texture = 0 )
{
	Option *button = new Option( caption );
	button->SetIcon( texture );
	options_.push_back( button );
	Add( button );

	// Update widest button.
	if ((widest_ == 0) || (button->GetWidth() > widest_)) {
		widest_ = button->GetWidth();
	}

	return button;
}

Option* Menu::getSelected()
{
	return selected_;
}

void Menu::pack()
{
	int width = widest_ + MENU_PADDING * 2 + MENU_STROKE * 2;
	int height = MENU_PADDING * 2 + MENU_STROKE * 2;

	// Pack buttons.
	for (int i = 0; i < options_.size(); i++) {
		Option *button = options_[i];

		// Pack this button.
		button->SetSize( widest_, button->GetHeight() );
		height += button->GetHeight();
	}

	// Add spacing heights.
	if (options_.size() >= 2) {
		height += (options_.size() - 1) * MENU_SPACING;
	}

	// Set new size.
	SetSize( width, height );
}

void Menu::display( int x, int y, Container *parent )
{
	setState( POPUP_STATE_ACTIVE );

	// Set new position, clamped to container.
	int newX = x, newY = y;
	if (newX + GetWidth() > parent->GetWidth()) {
		newX -= GetWidth();
	}

	// Set new Y.
	if (newY + GetHeight() > parent->GetHeight()) {
		newY -= GetHeight();
	}

	SetPosition( newX, newY );
}

void Menu::updatePosition()
{
	// Position all buttons.
	int y = GetY() + MENU_PADDING + MENU_STROKE;
	for (int i = 0; i < options_.size(); i++) {
		Option *button = options_[i];
		button->SetX( GetX() + MENU_PADDING + MENU_STROKE );
		button->SetY( y );

		y += button->GetHeight() + MENU_SPACING;
	}
}

bool Menu::OnMouseMoved( Mouse *mouse )
{
	// Pass message to buttons.
	for (int i = 0; i < options_.size(); i++) {
		Option *button = options_[i];
		button->OnMouseMoved( mouse );
	}

	return true;
}

bool Menu::OnLeftClicked( Mouse *mouse )
{
	selected_ = nullptr;
	if ( !mouse->isTouching( this ) ) {
		setState( POPUP_STATE_INACTIVE );
	}

	// Menu has priority over all under it.
	return true;
}

bool Menu::OnLeftReleased( Mouse *mouse )
{
	if (mouse->isTouching( this )) {
		for (int i = 0; i < options_.size(); i++) {
			Option *button = options_[i];

			// Set selected button, and go inactive.
			if (mouse->isTouching( button )) {
				setState( POPUP_STATE_INACTIVE );
				selected_ = button;
				break;
			}
		}

		return true;
	}

	return false;
}

bool Menu::OnRightClicked( Mouse *mouse )
{
	// Same behaviour as left click.
	return OnLeftClicked( mouse );
}

bool Menu::OnRightReleased( Mouse *mouse )
{
	// Same behaviour as left click.
	return OnLeftClicked( mouse );
}