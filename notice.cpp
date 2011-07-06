#include "notice.h"

// Static members.
Font* Notice::font_						= nullptr;

// Font constants.
const char* NOTICE_FONT_FACE			= "TF2 Build";
const unsigned int NOTICE_FONT_SIZE		= 20;
const bool NOTICE_FONT_BOLDED			= false;

// Notice specifications.
const unsigned int NOTICE_TEXT_WIDTH	= 300;
const unsigned int NOTICE_RADIUS		= 5;
const unsigned int NOTICE_PADDING		= 20;
const int NOTICE_SPACING				= 10;
const unsigned int NOTICE_STROKE_WIDTH	= 5;
const D3DCOLOR NOTICE_STROKE_COLOUR		= D3DCOLOR_RGBA( 153, 142, 121, 200 );
const D3DCOLOR NOTICE_COLOUR			= D3DCOLOR_XRGB( 42, 39, 37 );

Notice::Notice( const string& message ) : Popup( 0.0f, 0.0f )
{
	// Create rounded container.
	roundedContainer_ = new RoundedRectangleContainer( NOTICE_RADIUS, NOTICE_PADDING );
	RoundedRectangle *roundedRect = roundedContainer_->GetRoundedRectangle();
	roundedRect->SetStroke( NOTICE_STROKE_WIDTH, NOTICE_STROKE_COLOUR );
	roundedRect->SetStrokeType( STROKE_TYPE_OUTER );
	roundedRect->SetColour( NOTICE_COLOUR );
	roundedRect->RemoveTexture();
	Add( roundedContainer_ );
	SetConstraint( roundedContainer_, 0.0f, 0.0f );

	// Create layout for container.
	content_ = new VerticalLayout();
	content_->SetSpacing( NOTICE_SPACING );
	roundedContainer_->Add( content_ );
	roundedContainer_->SetContent( content_ );

	// Add default text to layout.
	text_ = new WrappedText( font_, NOTICE_TEXT_WIDTH );
	text_->SetTextFormatting( DT_CENTER );
	content_->Add( text_ );
	SetMessage( message );
	Pack();
}

Notice::~Notice()
{
	// Notice is destroyed.
}

void Notice::Pack( void )
{
	text_->Pack();
	content_->Pack();
	roundedContainer_->Pack();
	SetSize( roundedContainer_->GetWidth(), roundedContainer_->GetHeight() );
}

void Notice::SetMessage( const string& message )
{
	message_ = message;
	text_->SetText( message );
	Pack();
}

void Notice::AppendMessage( const string& message )
{
	SetMessage( message_ + message );
}

void Notice::Precache( DirectX *directX )
{
	font_ = directX->CreateFont( NOTICE_FONT_FACE, NOTICE_FONT_SIZE, NOTICE_FONT_BOLDED );
}

void Notice::Release( void )
{
	if (font_ != nullptr) {
		delete font_;
		font_ = nullptr;
	}
}