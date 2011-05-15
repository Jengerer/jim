#include "Option.h"

#define OPTION_PADDING_X	15
#define OPTION_PADDING_Y	10
#define OPTION_RADIUS		3

Option::Option( const string& caption ) : Button( caption ) 
{
	// Button constructed, same behaviour as parent.
}

Option::~Option( void )
{
	// Option destroyed.
}

void Option::OnDraw( DirectX *directX )
{
	// Set colour.
	D3DCOLOR buttonColour	= (isHovering_ ? BUTTON_COLOUR_HOVER : BUTTON_COLOUR );
	D3DCOLOR fontColour		= (isHovering_ ? BUTTON_FONT_HOVER : BUTTON_FONT_NORMAL );

	// Draw button base.
	float x = GetX();
	float y = GetY();
	directX->DrawRoundedRect( GetX(), GetY(), GetWidth(), GetHeight(), OPTION_RADIUS, buttonColour );

	// Draw text in center.
	RECT rect;
	rect.left = (long)x + OPTION_PADDING_X;
	rect.top = (long)y + OPTION_PADDING_Y;
	rect.right = (long)x + GetWidth() - OPTION_PADDING_X;
	rect.bottom = (long)y + GetHeight() - OPTION_PADDING_Y;

	// Write it.
	font_->drawText(
		caption_,
		&rect,
		DT_SINGLELINE | DT_VCENTER, 
		fontColour );
}

void Option::Pack( void )
{
	// TODO: Doesn't handle icons.
	RECT rect;
	font_->getTextRect( caption_, &rect, DT_SINGLELINE );
	int width = (rect.right - rect.left) + OPTION_PADDING_X * 2;
	int height = (rect.bottom - rect.top) + OPTION_PADDING_Y * 2;
	SetSize( width, height );
}