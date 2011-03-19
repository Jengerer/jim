#pragma once

#include <vector>

#include "Component.h"
#include "Font.h"

using namespace std;

#define BUTTON_RADIUS		5
#define BUTTON_PADDING_X	20
#define BUTTON_PADDING_Y	15
#define BUTTON_ICON_SPACING	10
#define BUTTON_ICON_SIZE	20

// Colours
#define BUTTON_COLOUR			D3DCOLOR_XRGB( 247, 231, 198 )
#define BUTTON_COLOUR_HOVER		D3DCOLOR_XRGB( 180, 81, 14 )
#define BUTTON_COLOUR_DISABLED	D3DCOLOR_ARGB( 50, 247, 231, 198 )
#define BUTTON_FONT_NORMAL		D3DCOLOR_XRGB( 118, 107, 94  )
#define BUTTON_FONT_HOVER		D3DCOLOR_XRGB( 42, 39, 37 )
#define BUTTON_FONT_DISABLED	D3DCOLOR_ARGB( 150, 42, 39, 37 )

enum EAlignment
{
	ALIGN_TOP_LEFT,
	ALIGN_TOP_RIGHT,
	ALIGN_BOTTOM_LEFT,
	ALIGN_BOTTOM_RIGHT
};

class Button: public Component
{
public:
	Button( const string& caption,
		Texture *texture,
		float x = 0.0f, float y = 0.0f,
		EAlignment align = ALIGN_TOP_LEFT );
	virtual ~Button();
	
	// Drawable functions.
	void draw( DirectX *directX );
	void setTexture( Texture *texture );

	// UI state handling.
	void enable();
	void disable();
	virtual void pack();
	bool isEnabled() const;

	// MouseListener functions.
	virtual bool mouseMoved( Mouse *mouse );

	// Class-wide texture.
	static Font*	font;

protected:
	string			caption_;
	bool			isHovering_;

private:
	Texture			*texture_;
	bool			enabled_;
};