#ifndef TOGGLE_SET_H
#define TOGGLE_SET_H

#include <vector>

#include <jui/ifont.h>
#include <jui/font_factory.h>
#include <jui/grid_layout.h>
#include <jui/horizontal_layout.h>
#include <jui/layout.h>
#include <jui/text.h>
#include <jui/vertical_layout.h>

#include "button.h"
#include "popup.h"
#include "rounded_rectangle.h"

typedef vector<Button*> ButtonList;

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

	virtual bool on_mouse_moved( Mouse *mouse );
	virtual bool on_mouse_clicked( Mouse *mouse );
	virtual bool on_mouse_released( Mouse *mouse );

	static void precache();
	static void release();

private:

	bool InSet( const ButtonList* set, Button *button ) const;
	void RemoveSet( ButtonList* set, Button *button );

protected:

	static IFont* titleFont_;
	static IFont* buttonFont_;

private:

	Text	*titleSetA_;
	Text	*titleSetB_;

	Button			*okayButton_;
	Button			*cancelButton_;

	VerticalLayout	*setLayout_;
	Layout			*layoutSetA_;
	Layout			*layoutSetB_;
	Layout			*buttonLayout_;

	RoundedRectangle *roundedRect_;

	ButtonList buttonSetA_;
	ButtonList buttonSetB_;

};

#endif // TOGGLE_SET_H