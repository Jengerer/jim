#include "ItemDisplay.h"

#define ITEM_DISPLAY_TITLE_FONT_FACE		"TF2 Build"
#define ITEM_DISPLAY_TITLE_FONT_SIZE		18
#define ITEM_DISPLAY_TITLE_FONT_BOLDED		false

#define ITEM_DISPLAY_COLOUR					D3DCOLOR_XRGB( 0, 0, 0 )

#define ITEM_DISPLAY_INFO_FONT_FACE			"TF2 Secondary"
#define ITEM_DISPLAY_INFO_FONT_SIZE			18
#define ITEM_DISPLAY_INFO_FONT_BOLDED		false

Font *ItemDisplay::nameFont_ = nullptr;
Font *ItemDisplay::infoFont_ = nullptr;

ItemDisplay::ItemDisplay()
{
	SetAlpha( 0 );

	// Unset texture.
	roundedRect_ = nullptr;

	// Create text objects.
	nameText_ = new WrappedText( nameFont_, ITEM_DISPLAY_WIDTH );
	nameText_->SetColour( ITEM_DISPLAY_NAME_COLOUR );
	nameText_->SetTextFormatting( DT_CENTER );
	infoText_ = new WrappedText( infoFont_, ITEM_DISPLAY_WIDTH );
	infoText_->SetTextFormatting( DT_CENTER );

	// Add to layout.
	textLayout_ = new VerticalLayout();
	textLayout_->SetSpacing( ITEM_DISPLAY_SPACING );
	textLayout_->Add( nameText_ );
	textLayout_->Add( infoText_ );
	Add( textLayout_ );

	roundedRect_ = new RoundedRectangle( GetWidth(), GetHeight(), ITEM_DISPLAY_RADIUS, ITEM_DISPLAY_COLOUR );
	roundedRect_->SetCornerRadius( ITEM_DISPLAY_RADIUS, ITEM_DISPLAY_RADIUS, ITEM_DISPLAY_RADIUS, ITEM_DISPLAY_RADIUS );
	AddBottom( roundedRect_ );
}

ItemDisplay::~ItemDisplay()
{
}

void ItemDisplay::UpdatePosition( void )
{
	roundedRect_->SetPosition( GetX(), GetY() );
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
	textLayout_->Pack();
	SetSize( 
		textLayout_->GetWidth() + ITEM_DISPLAY_PADDING * 2, 
		textLayout_->GetHeight() + ITEM_DISPLAY_PADDING * 2 );

	// Reset rounded rectangle size.
	roundedRect_->SetSize( GetWidth(), GetHeight() );
}

const Item* ItemDisplay::GetItem( void ) const
{
	return item_;
}

void ItemDisplay::SetItem( const Item *item )
{
	if (item_ != item) {
		item_ = item;
		if (item != nullptr) {
			SetActive( true );
			SetName( item->GetName() );

			// Build information text.
			stringstream infoStream;
			infoStream << "LEVEL " << (unsigned int)item->GetLevel() << " (" << hex << item->GetFlags() << ")";
			infoText_->SetText( infoStream.str() );
			Pack();
		}
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
	nameFont_ = directX->CreateFont( 
		ITEM_DISPLAY_TITLE_FONT_FACE, 
		ITEM_DISPLAY_TITLE_FONT_SIZE, 
		ITEM_DISPLAY_TITLE_FONT_BOLDED );

	infoFont_ = directX->CreateFont( 
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