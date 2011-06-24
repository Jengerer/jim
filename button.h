#pragma once

#include "container.h"
#include "font.h"
#include "imouse_handler.h"
#include "mouse.h"
#include "rounded_container.h"
#include "horizontal_layout.h"

class Button: public RoundedRectangleContainer, public IMouseHandler
{

public:

	Button( float localX = 0.0f, float localY = 0.0f );

	// Button content handling.
	Layout*			GetContentLayout() const;

	// Updating colour based on state.
	virtual void	Pack( void );
	void			UpdateButton( void );

	// UI state handling.
	void			SetEnabled( bool isEnabled );
	bool			IsEnabled( void ) const;

	// Mouse handling functions.
	virtual bool	OnMouseMoved( Mouse *mouse );
	virtual bool	OnLeftClicked( Mouse *mouse );
	virtual bool	OnLeftReleased( Mouse *mouse );

	bool			IsHovering( void ) const;

	static void		Precache( DirectX *directX );
	static void		Release();

	static Button*	CreateIconButton( Texture *texture );
	static Button*	CreateLabelButton( const string& label, Font* font = defaultFont_ );
	static Button*	CreateIconLabelButton( Texture *texture, const string& label, Font* font = defaultFont_ );

private:

	void			SetHovering( bool isHovering );

private:

	static Font			*defaultFont_;
	HorizontalLayout	*layout_;

	bool				isHovering_;
	bool				isEnabled_;

};