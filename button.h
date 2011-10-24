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
	virtual void	pack( void );
	void			UpdateButton( void );

	// UI state handling.
	void			SetEnabled( bool isEnabled );
	bool			IsEnabled( void ) const;

	// Mouse handling functions.
	virtual bool	on_mouse_moved( Mouse *mouse );
	virtual bool	on_mouse_clicked( Mouse *mouse );
	virtual bool	on_mouse_released( Mouse *mouse );

	bool			IsHovering( void ) const;

	static void		precache( Graphics2D* graphics );
	static void		Release();

	static Button*	CreateIconButton( Texture *texture );
	static Button*	CreateLabelButton( const std::string& label, Font* font = defaultFont_ );
	static Button*	CreateIconLabelButton( Texture *texture, const std::string& label, Font* font = defaultFont_ );

private:

	void			SetHovering( bool isHovering );

private:

	static Font			*defaultFont_;
	HorizontalLayout	*layout_;

	bool				isHovering_;
	bool				isEnabled_;

};