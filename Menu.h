#ifndef MENU_H
#define MENU_H

#include "Button.h"
#include "LabelButton.h"
#include "Popup.h"
#include "RoundedRectangle.h"
#include "VerticalLayout.h"

#define MENU_PADDING			10
#define MENU_SPACING			10
#define MENU_RADIUS				3
#define MENU_STROKE_SIZE		4
#define MENU_STROKE_COLOUR		D3DCOLOR_ARGB( 255, 255, 255, 50 )
#define MENU_BACKGROUND_COLOUR	D3DCOLOR_XRGB( 42, 39, 37 )

class Menu: public Popup, public IMouseHandler
{

public:

	Menu( void );
	virtual ~Menu( void);

	// Menu handling functions.
	LabelButton*	AddOption( const string& caption, Texture *texture = nullptr );
	LabelButton*	GetClicked( void ) const;

	// Drawing functions.
	virtual void	Pack( void );
	virtual void	UpdatePosition( void );

	// Mouse handling.
	virtual bool	OnMouseMoved( Mouse *mouse );
	virtual bool	OnLeftClicked( Mouse *mouse );
	virtual bool	OnLeftReleased( Mouse *mouse );

private:

	void			SetClicked( LabelButton *clicked );

private:

	RoundedRectangle		*roundedRect_;
	VerticalLayout			*layout_;
	LabelButton*			clicked_;
	vector<LabelButton*>	options_;

};

#endif // MENU_H