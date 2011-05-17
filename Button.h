#pragma once

#include <vector>

#include "IMouseHandler.h"
#include "Component.h"
#include "Mouse.h"
#include "Font.h"

using namespace std;

#define BUTTON_RADIUS		5
#define BUTTON_PADDING_X	15
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

class Button: public Component, public IMouseHandler
{

public:

	Button( const string& caption, float x = 0.0f, float y = 0.0f );
	virtual ~Button( void );
	
	// Drawable functions.
	void			OnDraw( DirectX *directX );

	// Optional icon setting/getting.
	void			SetIcon( Texture *texture );
	Texture*		GetIcon( void );

	// Caption manipulation.
	void			SetCaption( const string& caption );
	const string&	GetCaption( void ) const;

	// Compresses the text and caption and fits to size.
	virtual void	Pack( void );

	// UI state handling.
	void SetEnabled( bool isEnabled );
	bool IsEnabled( void ) const;

	// Mouse handling functions.
	virtual bool OnMouseMoved( Mouse *mouse );
	virtual bool OnLeftClicked( Mouse *mouse );
	virtual bool OnLeftReleased( Mouse *mouse );

	static void		Precache( DirectX *directX );
	static void		Release( void );

protected:

	void	SetHovering( bool isHovering );
	bool	IsHovering( void ) const;

protected:

	string			caption_;
	bool			isHovering_;
	static Font		*font_;

private:

	// TODO: Maybe make icon an image instead of just a texture?
	Texture			*icon_;
	bool			isEnabled_;

};