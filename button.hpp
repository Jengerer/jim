#ifndef BUTTON_HPP
#define BUTTON_HPP

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

    Button( void );
	Button( int x, int y );

	// Layout handling.
    bool initialize( const JUTIL::String* label, const JUI::Texture* icon );

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

#endif // BUTTON_HPP