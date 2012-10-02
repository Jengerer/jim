#ifndef ITEM_DISPLAY_H
#define ITEM_DISPLAY_H

#include <jui/layout/container.hpp>
#include <jui/gfx/font_interface.hpp>
#include <jui/graphics_2d.h>
#include <jui/text.hpp>
#include <jui/wrapped_text.h>
#include <jui/vertical_layout.hpp>

#include "item.hpp"
#include "rounded_rectangle_container.hpp"

/*
 * Class for displaying item information.
 */
class ItemDisplay : public RoundedRectangleContainer
{

public:

	// Constructor.
	ItemDisplay( void );
	virtual ~ItemDisplay( void );

	// Drawing functions.
	void			update_display();
	void			update_alpha();
	virtual void	pack();

	// Item functions.
	const Item*		get_item( void ) const;
	void			set_item( const Item *item );

	// Getting item name.
	const std::string&	get_name( void ) const;

	// Activity functions.
	bool			is_active( void ) const;
	void			set_active( bool is_active );

	// Load and release display resources.
	static void		precache();
	static void		release();

private:

	// Name setter.
	void			set_name( const std::string& name );

protected:

	// Shared fonts.
	static JUI::FontInterface *name_font_;
	static JUI::FontInterface *info_font_;

private:

	// Attributes.
	bool			is_active_;
	const Item*		item_;
	std::string		item_name_;

	// Layout.
	JUI::VerticalLayout	*text_layout_;
	JUI::WrappedText		*name_text_;
	JUI::WrappedText		*info_text_;

};

#endif // ITEM_DISPLAY_H