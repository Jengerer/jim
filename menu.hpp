#ifndef MENU_H
#define MENU_H

#include <jui/vertical_layout.hpp>

#include "button.hpp"
#include "popup.hpp"
#include "rounded_rectangle.hpp"

class Menu: public Popup
{

public:

	Menu( void );
	virtual ~Menu( void);

	// Menu handling functions.
	Button*	AddOption( const std::string& caption );
	Button*	GetClicked( void ) const;

	// Drawing functions.
	virtual void	pack( void );

	// JUI::Mouse* handling.
	virtual bool	on_mouse_moved( JUI::Mouse* mouse );
	virtual bool	on_mouse_clicked( JUI::Mouse* mouse );
	virtual bool	on_mouse_released( JUI::Mouse* mouse );

private:

	void			set_clicked( Button *clicked );

private:

	RoundedRectangle		*rounded_rect_;
	JUI::VerticalLayout			*layout_;
	Button*			clicked_;
	vector<Button*>	options_;

};

#endif // MENU_H