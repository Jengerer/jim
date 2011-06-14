#include "item_display.h"

#define ITEM_DISPLAY_TITLE_FONT_FACE		"TF2 Build"
#define ITEM_DISPLAY_TITLE_FONT_SIZE		17
#define ITEM_DISPLAY_TITLE_FONT_BOLDED		false

#define ITEM_DISPLAY_COLOUR					D3DCOLOR_XRGB( 0, 0, 0 )

#define ITEM_DISPLAY_INFO_FONT_FACE			"TF2 Secondary"
#define ITEM_DISPLAY_INFO_FONT_SIZE			16
#define ITEM_DISPLAY_INFO_FONT_BOLDED		false

#define ITEM_DISPLAY_NORMAL_COLOUR	D3DCOLOR_XRGB( 248, 212, 0 )
#define ITEM_DISPLAY_VINTAGE_COLOUR	D3DCOLOR_XRGB( 69, 97, 141 )
#define ITEM_DISPLAY_GENUINE_COLOUR	D3DCOLOR_XRGB( 75, 115, 83 )

Font *ItemDisplay::nameFont_ = nullptr;
Font *ItemDisplay::infoFont_ = nullptr;

ItemDisplay::ItemDisplay( void ) : RoundedRectangleContainer( ITEM_DISPLAY_RADIUS )
{
	SetAlpha( 0 );
	SetPadding( ITEM_DISPLAY_PADDING );
	SetColour( ITEM_DISPLAY_COLOUR );
	SetItem( nullptr );

	// Create text objects.
	nameText_ = new WrappedText( nameFont_, ITEM_DISPLAY_WIDTH );
	nameText_->SetColour( ITEM_DISPLAY_NAME_COLOUR );
	nameText_->SetTextFormatting( DT_CENTER );
	infoText_ = new WrappedText( infoFont_, ITEM_DISPLAY_WIDTH );
	infoText_->SetTextFormatting( DT_CENTER );

	// Add to layout.
	textLayout_ = new VerticalLayout();
	textLayout_->SetSpacing( ITEM_DISPLAY_SPACING );
	textLayout_->SetLocalPosition( ITEM_DISPLAY_PADDING, ITEM_DISPLAY_PADDING );
	textLayout_->Add( nameText_ );
	textLayout_->Add( infoText_ );

	// Pack so we can create a temporary rectangle.
	Add( textLayout_ );
	SetContained( textLayout_ );
	Pack();
}

ItemDisplay::~ItemDisplay( void )
{
}

void ItemDisplay::UpdateAlpha( void )
{
	if ( IsActive() ) {
		SetAlpha( GetAlpha() + ITEM_DISPLAY_ALPHA_SPEED );
		if (GetAlpha() > ITEM_DISPLAY_ALPHA_MAX) {
			SetAlpha( ITEM_DISPLAY_ALPHA_MAX );
		}
	}
	else {
		SetAlpha( GetAlpha() - ITEM_DISPLAY_ALPHA_SPEED );
	}
}

void ItemDisplay::Pack( void )
{
	nameText_->Pack();
	infoText_->Pack();
	textLayout_->Pack();
	RoundedRectangleContainer::Pack();
	UpdateChildren();
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

			// Alter display based on quality.
			string namePrefix;
			D3DCOLOR nameColour;
			switch (item->GetQuality()) {
			case EItemQuality::k_EItemQuality_Common:
				namePrefix = "Genuine";
				nameColour = ITEM_DISPLAY_GENUINE_COLOUR;
				break;

			case EItemQuality::k_EItemQuality_Unique:
				namePrefix = "Vintage";
				nameColour = ITEM_DISPLAY_VINTAGE_COLOUR;
				break;

			default:
				nameColour = ITEM_DISPLAY_NORMAL_COLOUR;
				break;
			}

			// Set quality colour and prefix.
			nameText_->SetColour( nameColour );
			if (item->HasCustomName()) {
				SetName( item->GetCustomName() );
			}
			else {
				SetName( namePrefix + ' ' + item->GetName() );
			}

			// Build information text.
			stringstream infoStream;
			infoStream << "LEVEL " << (uint16)item->GetLevel();
#if defined( _DEBUG )
			infoStream << "\nFLAGS: " << hex << item->GetFlags();
#endif
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