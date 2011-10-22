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
	Button*	AddOption( const std::string& caption );
	Button*	GetClicked( void ) const;

	// Drawing functions.
	virtual void	Pack( void );

	// Mouse handling.
	virtual bool	on_mouse_moved( Mouse *mouse );
	virtual bool	on_mouse_clicked( Mouse *mouse );
	virtual bool	on_mouse_released( Mouse *mouse );

private:

	void			SetClicked( Button *clicked );

private:

	RoundedRectangle		*roundedRect_;
	VerticalLayout			*layout_;
	Button*			clicked_;
	vector<Button*>	options_;

};

#endif // MENU_H