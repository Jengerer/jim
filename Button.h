#pragma once

#include "Container.h"
#include "Font.h"
#include "IMouseHandler.h"
#include "Mouse.h"
#include "RoundedRectangleContainer.h"
#include "HorizontalLayout.h"

class Button: public RoundedRectangleContainer, public IMouseHandler
{

public:

	Button( float x = 0.0f, float y = 0.0f );
	virtual ~Button( void );

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

private:

	void			SetHovering( bool isHovering );
	bool			IsHovering( void ) const;

protected:

	static Font			*font_;
	HorizontalLayout	*layout_;

private:

	bool				isHovering_;
	bool				isEnabled_;

};