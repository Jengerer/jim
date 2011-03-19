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
	directX->drawRoundedRect( getX(), getY(), getWidth(), getHeight(), MENU_RADIUS + MENU_STROKE, MENU_STROKE_COLOUR );
	directX->drawRoundedRect( getX() + MENU_STROKE, getY() + MENU_STROKE, getWidth() - MENU_STROKE * 2, getHeight() - MENU_STROKE * 2, MENU_RADIUS, MENU_BACKGROUND_COLOUR );

	// Draw buttons.
	for (int i = 0; i < options_.size(); i++) {
		Option *button = options_[i];
		button->draw( directX );
	}
}

Option* Menu::addOption( const string& caption, Texture *texture = 0 )
{
	Option *button = new Option( caption, texture );
	options_.push_back( button );
	add( button );

	// Update widest button.
	if ((widest_ == 0) || (button->getWidth() > widest_)) {
		widest_ = button->getWidth();
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
		button->setSize( widest_, button->getHeight() );
		height += button->getHeight();
	}

	// Add spacing heights.
	if (options_.size() >= 2) {
		height += (options_.size() - 1) * MENU_SPACING;
	}

	// Set new size.
	setSize( width, height );
}

void Menu::display( int x, int y, Container *parent )
{
	setState( POPUP_STATE_ACTIVE );

	// Set new position, clamped to container.
	int newX = x, newY = y;
	if (newX + getWidth() > parent->getWidth()) {
		newX -= getWidth();
	}

	// Set new Y.
	if (newY + getHeight() > parent->getHeight()) {
		newY -= getHeight();
	}

	setPosition( newX, newY );
}

void Menu::updatePosition()
{
	// Position all buttons.
	int y = getY() + MENU_PADDING + MENU_STROKE;
	for (int i = 0; i < options_.size(); i++) {
		Option *button = options_[i];
		button->setX( getX() + MENU_PADDING + MENU_STROKE );
		button->setY( y );

		y += button->getHeight() + MENU_SPACING;
	}
}

bool Menu::leftClicked( Mouse *mouse )
{
	// Close popup if no buttons clicked.
	selected_ = 0;
	if (!mouse->isTouching( this )) {
		setState( POPUP_STATE_INACTIVE );
		return false;
	}

	return true;
}

bool Menu::rightClicked( Mouse *mouse )
{
	// Close popup if not clicked.
	selected_ = 0;
	if (!mouse->isTouching( this )) {
		setState( POPUP_STATE_INACTIVE );
		return false;
	}

	return true;
}

bool Menu::leftReleased( Mouse *mouse )
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
}

bool Menu::mouseMoved( Mouse *mouse )
{
	// Pass message to buttons.
	for (int i = 0; i < options_.size(); i++) {
		Option *button = options_[i];
		button->mouseMoved( mouse );
	}

	return true;
}