#include "ItemDisplay.h"

#define ITEM_DISPLAY_TITLE_FONT_FACE		"TF2 Build"
#define ITEM_DISPLAY_TITLE_FONT_SIZE		18
#define ITEM_DISPLAY_TITLE_FONT_BOLDED		false

#define ITEM_DISPLAY_INFO_FONT_FACE			"TF2 Secondary"
#define ITEM_DISPLAY_INFO_FONT_SIZE			18
#define ITEM_DISPLAY_INFO_FONT_BOLDED		false

Font *ItemDisplay::nameFont_ = nullptr;
Font *ItemDisplay::infoFont_ = nullptr;

ItemDisplay::ItemDisplay()
{
	SetAlpha( 0 );

	// Create text objects.
	nameText_ = new Text( nameFont_ );
	nameText_->SetColour( ITEM_DISPLAY_NAME_COLOUR );
	infoText_ = new Text( infoFont_ );

	// Add to layout.
	textLayout_ = new VerticalLayout();
	textLayout_->SetSpacing( ITEM_DISPLAY_SPACING );
	textLayout_->Add( nameText_ );
	textLayout_->Add( infoText_ );
	textLayout_->Pack();
	Add( textLayout_ );
}

ItemDisplay::~ItemDisplay()
{
	// Item display destroyed.
}

void ItemDisplay::OnDraw( DirectX *directX )
{
	// Transition alpha.
	if (item_ != nullptr) {
		if (GetAlpha() > 0) {
			// Draw base rectangle.
			directX->drawRoundedRect( GetX(), GetY(), GetWidth(), GetHeight(), 5, D3DCOLOR_RGBA( 0, 0, 0, alpha_ ) );
			textLayout_->OnDraw( directX );
		}
	}
}

void ItemDisplay::UpdatePosition( void )
{
	textLayout_->SetPosition( GetX() + ITEM_DISPLAY_PADDING, GetY() + ITEM_DISPLAY_PADDING );
}

void ItemDisplay::UpdateAlpha( void )
{
	if ( IsActive() ) {
		SetAlpha( GetAlpha() + ITEM_DISPLAY_ALPHA_SPEED );
	}
	else {
		SetAlpha( GetAlpha() - ITEM_DISPLAY_ALPHA_SPEED );
	}
}

void ItemDisplay::Pack( void )
{
	SetSize( textLayout_->GetWidth() + ITEM_DISPLAY_PADDING * 2, textLayout_->GetHeight() + ITEM_DISPLAY_PADDING * 2 );
}

void ItemDisplay::SetAlpha( int alpha )
{
	alpha_ = alpha;
	if (alpha_ < 0) {
		alpha_ = 0;
	}
	else if (alpha_ > ITEM_DISPLAY_ALPHA_MAX) {
		alpha_ = ITEM_DISPLAY_ALPHA_MAX;
	}
}

int ItemDisplay::GetAlpha( void ) const
{
	return alpha_;
}

const Item* ItemDisplay::GetItem( void ) const
{
	return item_;
}

void ItemDisplay::SetItem( const Item *item )
{
	item_ = item;
	if (item != nullptr) {
		SetActive( true );
		SetName( item->GetName() );

		// Build information text.
		stringstream infoStream;
		infoStream << "LEVEL " << (unsigned int)item->GetLevel() << " (" << hex << item->GetFlags() << ")";
		infoText_->SetText( infoStream.str() );

		textLayout_->Pack();
		Pack();
	}
}

const string& ItemDisplay::GetName( void ) const
{
	return itemName_;
}

void ItemDisplay::SetName( const string& name )
{
	itemName_ = name;
	nameText_->SetText( name );
}

bool ItemDisplay::IsActive( void ) const
{
	return isActive_;
}

void ItemDisplay::SetActive( bool isActive )
{
	isActive_ = isActive;
}

void ItemDisplay::Precache( DirectX* directX )
{
	nameFont_ = directX->createFont( 
		ITEM_DISPLAY_TITLE_FONT_FACE, 
		ITEM_DISPLAY_TITLE_FONT_SIZE, 
		ITEM_DISPLAY_TITLE_FONT_BOLDED );

	infoFont_ = directX->createFont( 
		ITEM_DISPLAY_INFO_FONT_FACE, 
		ITEM_DISPLAY_INFO_FONT_SIZE, 
		ITEM_DISPLAY_INFO_FONT_BOLDED );
}

void ItemDisplay::Release( void )
{
	if (nameFont_ != nullptr) {
		delete nameFont_;
		nameFont_ = nullptr;
	}

	if (infoFont_ != nullptr ) {
		delete infoFont_;
		infoFont_ = nullptr;
	}
}