#include "label_button.h"

#define LABEL_BUTTON_FONT_NAME		"TF2 Build"
#define LABEL_BUTTON_FONT_SIZE		20
#define LABEL_BUTTON_FONT_BOLDED	false
#define LABEL_BUTTON_FONT_COLOUR	D3DCOLOR_XRGB( 42, 39, 37 )

#define BUTTON_FONT_NORMAL		D3DCOLOR_XRGB( 118, 107, 94  )
#define BUTTON_FONT_HOVER		D3DCOLOR_XRGB( 42, 39, 37 )
#define BUTTON_FONT_DISABLED	D3DCOLOR_ARGB( 150, 42, 39, 37 )

Font *LabelButton::font_ = nullptr;

LabelButton::LabelButton( const string& label, Texture *icon, float x, float y ) : IconButton( icon, x, y )
{
	label_ = new Text( font_ );
	label_->SetColour( LABEL_BUTTON_FONT_COLOUR );
	layout_->Add( label_);
	SetLabel( label );
}

LabelButton::~LabelButton( void )
{
	// Label button destroyed.
}

void LabelButton::Pack( void )
{
	label_->Pack();
	Button::Pack();
}

void LabelButton::SetLabel( const string& caption )
{
	label_->SetText( caption );
}

void LabelButton::Precache( DirectX *directX )
{
	font_ = directX->CreateFont( LABEL_BUTTON_FONT_NAME, LABEL_BUTTON_FONT_SIZE, LABEL_BUTTON_FONT_BOLDED );
}

void LabelButton::Release( void )
{
	if (font_ != nullptr) {
		delete font_;
		font_ = nullptr;
	}
}

LabelButton* LabelButton::Create( const string& label, Texture *icon, float x, float y )
{
	LabelButton* labelButton = new LabelButton( label, icon, x, y );
	labelButton->SetEnabled( true );
	labelButton->Pack();
	return labelButton;
}