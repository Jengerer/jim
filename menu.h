#ifndef MENU_H
#define MENU_H

#include "button.h"
#include "popup.h"
#include "rounded_rectangle.h"
#include "vertical_layout.h"

#define MENU_PADDING			10
#define MENU_SPACING			10
#define MENU_RADIUS				3
#define MENU_STROKE_SIZE		4
#define MENU_STROKE_COLOUR		D3DCOLOR_ARGB( 255, 255, 255, 50 )
#define MENU_BACKGROUND_COLOUR	D3DCOLOR_XRGB( 42, 39, 37 )

class Menu: public Popup
{

public:

	Menu( void );
	virtual ~Menu( void);

	// Menu handling functions.
	Button*	AddOption( const string& caption );
	Button*	GetClicked( void ) const;

	// Drawing functions.
	virtual void	Pack( void );

	// Mouse handling.
	virtual bool	OnMouseMoved( Mouse *mouse );
	virtual bool	OnLeftClicked( Mouse *mouse );
	virtual bool	OnLeftReleased( Mouse *mouse );

private:

	void			SetClicked( Button *clicked );

private:

	RoundedRectangle		*roundedRect_;
	VerticalLayout			*layout_;
	Button*			clicked_;
	vector<Button*>	options_;

};

#endif // MENU_H