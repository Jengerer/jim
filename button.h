#ifndef BUTTON_H
#define BUTTON_H

#include <jui/container.h>
#include <jui/ifont.h>
#include <jui/font_factory.h>
#include <jui/imouse_handler.h>
#include <jui/mouse.h>
#include <jui/horizontal_layout.h>

#include "rounded_rectangle_container.h"

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
	static void		release();

	static Button*	CreateIconButton( Texture *texture );
	static Button*	create_label_button( const std::string& label, IFont* font = defaultFont_ );
	static Button*	CreateIconLabelButton( Texture *texture, const std::string& label, IFont* font = defaultFont_ );

private:

	void			SetHovering( bool isHovering );

private:

	static IFont		*defaultFont_;
	HorizontalLayout	*layout_;

	bool				isHovering_;
	bool				isEnabled_;

};

#endif // BUTTON_H