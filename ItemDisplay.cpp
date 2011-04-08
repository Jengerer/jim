#include "ItemDisplay.h"

Font *ItemDisplay::nameFont = 0;
Font *ItemDisplay::infoFont = 0;

ItemDisplay::ItemDisplay()
{
	// Item display created.
	setItem( 0 );
	alpha_ = 0;
}

ItemDisplay::~ItemDisplay()
{
	// Item display destroyed.
}

void ItemDisplay::draw( DirectX *directX )
{
	// Transition alpha.
	if (item_ != 0) {
		if (isActive_) {
			alpha_ += DISPLAY_ALPHA_SPEED;
			if (alpha_ > DISPLAY_ALPHA_MAX) {
				alpha_ = DISPLAY_ALPHA_MAX;
			}
		}
		else {
			alpha_ -= DISPLAY_ALPHA_SPEED;
			if (alpha_ < 0) {
				alpha_ = 0;
			}
		}

		if (alpha_ > 0) {
			// Draw base rectangle.
			directX->drawRoundedRect( getX(), getY(), DISPLAY_WIDTH, getHeight(), 5, D3DCOLOR_RGBA( 0, 0, 0, alpha_ ) );

			// Now draw name centered.
			RECT titleRect = { getX() + DISPLAY_PADDING, getY() + DISPLAY_PADDING, getX() + DISPLAY_WIDTH - DISPLAY_PADDING, getY() + getHeight() - DISPLAY_PADDING };
			nameFont->getTextRect( itemName, &titleRect, DT_CENTER );
			nameFont->drawText( itemName, &titleRect, DT_CENTER, D3DCOLOR_RGBA( 255, 255, 0, alpha_ ) );

			// Draw level below it.
			RECT levelRect = { getX() + DISPLAY_PADDING, titleRect.bottom + DISPLAY_SPACING, getX() + DISPLAY_WIDTH - DISPLAY_PADDING, getY() + getHeight() - DISPLAY_PADDING };
			stringstream levelText;
			levelText << "LEVEL " << (unsigned int)item_->getLevel() << " (" << hex << item_->getFlags() << ")";
			infoFont->drawText( levelText.str().c_str(), &levelRect, DT_CENTER | DT_WORDBREAK, D3DCOLOR_RGBA( 255, 255, 255, alpha_ ) );
		}
	}
}

Item *ItemDisplay::getItem()
{
	return item_;
}

void ItemDisplay::setItem( Item *item )
{
	// Basic set.
	item_ = item;

	// Now resize for information.
	if (item != 0) {
		RECT titleRect = { 0, 0, DISPLAY_WIDTH - DISPLAY_PADDING*2, 0 };
		itemName = item->getName();
		nameFont->wrapText( itemName, &titleRect, DT_CENTER );

		RECT levelRect = { 0, 0, DISPLAY_WIDTH - DISPLAY_PADDING*2, 0 };
		stringstream levelText;
		levelText << "LEVEL " << (unsigned int)item->getLevel() << " (" << hex << item->getFlags() << ")";
		infoFont->getTextRect( levelText.str().c_str(), &levelRect, DT_WORDBREAK );

		// Resize display.
		setSize( DISPLAY_WIDTH, (titleRect.bottom - titleRect.top) + DISPLAY_SPACING + (levelRect.bottom - levelRect.top) + DISPLAY_PADDING * 2 );
	}
}

void ItemDisplay::setActive( bool isActive )
{
	isActive_ = isActive;
}