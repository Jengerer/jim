#ifndef BUTTON_H
#define BUTTON_H

#include <jui/layout/container.hpp>
#include <jui/gfx/font_interface.hpp>
#include <jui/gfx/font_factory.hpp>
#include <jui/io/mouse_handler_interface.hpp>
#include <jui/io/mouse.hpp>
#include <jui/layout/horizontal_layout.hpp>

#include "rounded_rectangle_container.hpp"

class Button: public RoundedRectangleContainer, public JUI::MouseHandlerInterface
{

public:

	Button( float localX = 0.0f, float localY = 0.0f );

	// Button content handling.
	JUI::Layout*    GetContentLayout() const;

	// Updating colour based on state.
	virtual void	pack( void );
	void			UpdateButton( void );

	// UI state handling.
	void			SetEnabled( bool isEnabled );
	bool			IsEnabled( void ) const;

	// JUI::Mouse* handling functions.
	virtual bool	on_mouse_moved( JUI::Mouse* mouse );
	virtual bool	on_mouse_clicked( JUI::Mouse* mouse );
	virtual bool	on_mouse_released( JUI::Mouse* mouse );

	bool			IsHovering( void ) const;

	static void		precache( JUI::Graphics2D* graphics );
	static void		release();

	static Button*	CreateIconButton( JUI::Texture *texture );
	static Button*	create_label_button( const std::string& label, JUI::FontInterface* font = defaultFont_ );
	static Button*	CreateIconLabelButton( JUI::Texture *texture, const std::string& label, JUI::FontInterface* font = defaultFont_ );

private:

	void			SetHovering( bool isHovering );

private:

	static JUI::FontInterface		*defaultFont_;
	JUI::HorizontalLayout	*layout_;

	bool				isHovering_;
	bool				isEnabled_;

};

#endif // BUTTON_H