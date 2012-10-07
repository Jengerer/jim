#ifndef ITEM_DISPLAY_HPP
#define ITEM_DISPLAY_HPP

#include <jui/gfx/font_interface.hpp>
#include <jui/gfx/graphics_2d.hpp>
#include <jui/gfx/text.hpp>
#include <jui/gfx/wrapped_text.hpp>
#include <jui/layout/container.hpp>
#include <jui/layout/vertical_layout.hpp>

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

	// Element initialization.
	bool initialize( void );

	// Drawing functions.
	void update_display( void );
	void update_alpha( void );
	virtual void pack( void );

	// Item functions.
	const Item* get_item( void ) const;
	bool set_item( const Item *item );

	// Getting item name.
	const JUTIL::String* get_name( void ) const;

	// Activity functions.
	bool is_active( void ) const;
	void set_active( bool is_active );

	// Load and release display resources.
	static bool precache( void );
	static void release( void );

private:

	// Name setter.
	void set_name( const JUTIL::ConstantString& name );

protected:

	// Shared fonts.
	static JUI::FontInterface *name_font_;
	static JUI::FontInterface *info_font_;

private:

	// Attributes.
	bool is_active_;
	const Item* item_;
	JUTIL::String item_name_;

	// Layout.
	JUI::VerticalLayout* text_layout_;
	JUI::WrappedText* name_text_;
	JUI::WrappedText* info_text_;

};

#endif // ITEM_DISPLAY_HPP