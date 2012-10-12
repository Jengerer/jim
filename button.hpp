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

	Button( float x = 0.0f, float y = 0.0f );

	// Layout handling.
    bool initialize( void );
	JUI::Layout* get_content_layout( void ) const;

	// Updating colour based on state.
	virtual void pack( void );
	void update_button( void );

	// UI state handling.
	void set_enabled( bool is_enabled );
	bool is_enabled( void ) const;
	bool is_hovering( void ) const;

	// Mouse handling functions.
	virtual JUI::IOResult on_mouse_moved( JUI::Mouse* mouse );
	virtual JUI::IOResult on_mouse_clicked( JUI::Mouse* mouse );
	virtual JUI::IOResult on_mouse_released( JUI::Mouse* mouse );

    // Resource loading.
	static bool precache( JUI::Graphics2D* graphics );
	static void release( void );

    // Button creation.
	static Button* create_icon_button( JUI::Texture *texture );
	static Button* create_label_button( const JUTIL::String* label, JUI::FontInterface* font = default_font_ );
	static Button* create_icon_label_button( JUI::Texture *texture, const JUTIL::String* label, JUI::FontInterface* font = default_font_ );

private:

    // Button generic creation.
    static Button* create_generic_button( void );

    // Update hover state.
    void set_hovering( bool is_hovering );

private:

    // Instance-generic members.
	static JUI::FontInterface* default_font_;

private:

	JUI::HorizontalLayout* layout_;

    // UI state members.
	bool is_hovering_;
	bool is_enabled_;

};

#endif // BUTTON_H