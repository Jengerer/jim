#ifndef MENU_H
#define MENU_H

#include "button.h"
#include "popup.h"
#include "rounded_rectangle.h"
#include "vertical_layout.h"

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
	virtual bool	MouseMoved( Mouse *mouse );
	virtual bool	MouseClicked( Mouse *mouse );
	virtual bool	MouseReleased( Mouse *mouse );

private:

	void			SetClicked( Button *clicked );

private:

	RoundedRectangle		*roundedRect_;
	VerticalLayout			*layout_;
	Button*			clicked_;
	vector<Button*>	options_;

};

#endif // MENU_H