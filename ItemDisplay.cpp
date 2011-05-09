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

void ItemDisplay::OnDraw( DirectX *directX )
{
	// Transition alpha.
	if (item_ != nullptr) {
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
			directX->drawRoundedRect( GetX(), GetY(), DISPLAY_WIDTH, GetHeight(), 5, D3DCOLOR_RGBA( 0, 0, 0, alpha_ ) );

			// Now draw name centered.
			RECT titleRect = { GetX() + DISPLAY_PADDING, GetY() + DISPLAY_PADDING, GetX() + DISPLAY_WIDTH - DISPLAY_PADDING, GetY() + GetHeight() - DISPLAY_PADDING };
			nameFont->getTextRect( itemName, &titleRect, DT_CENTER );
			nameFont->drawText( itemName, &titleRect, DT_CENTER, D3DCOLOR_RGBA( 255, 255, 0, alpha_ ) );

			// Draw level below it.
			RECT levelRect = { GetX() + DISPLAY_PADDING, titleRect.bottom + DISPLAY_SPACING, GetX() + DISPLAY_WIDTH - DISPLAY_PADDING, GetY() + GetHeight() - DISPLAY_PADDING };
			stringstream levelText;
			levelText << "LEVEL " << (unsigned int)item_->GetLevel() << " (" << hex << item_->GetFlags() << ")";
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
		itemName = item->GetName();
		nameFont->wrapText( itemName, &titleRect, DT_CENTER );

		RECT levelRect = { 0, 0, DISPLAY_WIDTH - DISPLAY_PADDING*2, 0 };
		stringstream levelText;
		levelText << "LEVEL " << (unsigned int)item->GetLevel() << " (" << hex << item->GetFlags() << ")";
		infoFont->getTextRect( levelText.str().c_str(), &levelRect, DT_WORDBREAK );

		// Resize display.
		SetSize( DISPLAY_WIDTH, (titleRect.bottom - titleRect.top) + DISPLAY_SPACING + (levelRect.bottom - levelRect.top) + DISPLAY_PADDING * 2 );
	}
}

void ItemDisplay::setActive( bool isActive )
{
	isActive_ = isActive;
}