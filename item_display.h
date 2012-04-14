#ifndef ITEM_DISPLAY_H
#define ITEM_DISPLAY_H

#include <jui/container.h>
#include <jui/ifont.h>
#include <jui/graphics_2d.h>
#include <jui/text.h>
#include <jui/wrapped_text.h>
#include <jui/vertical_layout.h>

#include "item.h"
#include "rounded_rectangle_container.h"

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
	static IFont *name_font_;
	static IFont *info_font_;

private:

	// Attributes.
	bool			is_active_;
	const Item*		item_;
	std::string		item_name_;

	// Layout.
	VerticalLayout	*text_layout_;
	WrappedText		*name_text_;
	WrappedText		*info_text_;

};

#endif // ITEM_DISPLAY_H