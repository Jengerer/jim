#ifndef TOGGLE_SET_H
#define TOGGLE_SET_H

#include <jui/gfx/font_interface.hpp>
#include <jui/gfx/font_factory.hpp>
#include <jui/gfx/text.hpp>
#include <jui/layout/grid_layout.hpp>
#include <jui/layout/horizontal_layout.hpp>
#include <jui/layout/layout.hpp>
#include <jui/layout/vertical_layout.hpp>

#include "button.hpp"
#include "popup.hpp"
#include "rounded_rectangle.hpp"

class ToggleSet : public Popup
{

public:

	ToggleSet( const std::string& nameSetA, const std::string& nameSetB, float x = 0.0f, float y = 0.0f );
	virtual ~ToggleSet( void );

	virtual void pack( void );

	void	add_set_a( Button *button );
	void	add_set_b( Button *button );

	void	remove_set_a( Button *button );
	void	remove_set_b( Button *button );

	bool	in_set_a( Button *button ) const;
	bool	in_set_b( Button *button ) const;

	virtual bool on_mouse_moved( JUI::Mouse* mouse );
	virtual bool on_mouse_clicked( JUI::Mouse* mouse );
	virtual bool on_mouse_released( JUI::Mouse* mouse );

	static void precache();
	static void release();

private:

	bool InSet( const ButtonList* set, Button *button ) const;
	void RemoveSet( ButtonList* set, Button *button );

protected:

	static JUI::FontInterface* title_font_;
	static JUI::FontInterface* buttonFont_;

private:

	JUI::Text	*titleSetA_;
	JUI::Text	*titleSetB_;

	Button			*okayButton_;
	Button			*cancelButton_;

	JUI::VerticalLayout	*setLayout_;
	JUI::Layout			*layoutSetA_;
	JUI::Layout			*layoutSetB_;
	JUI::Layout			*buttonLayout_;

	RoundedRectangle *rounded_rect_;

	ButtonList buttonSetA_;
	ButtonList buttonSetB_;

};

#endif // TOGGLE_SET_H