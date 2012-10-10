#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <jui/layout/container.hpp>
#include <jui/layout/horizontal_layout.hpp>
#include <jui/gfx/font_interface.hpp>
#include <jui/gfx/font_factory.hpp>
#include <jui/gfx/image.hpp>
#include <jui/gfx/texture.hpp>
#include <jui/gfx/wrapped_text.hpp>

#include "rounded_rectangle_container.hpp"

class Notification : public RoundedRectangleContainer
{
	
public:

	Notification( const JUTIL::String* message, const JUI::Texture *texture = nullptr );
	virtual ~Notification( void );

    // Notification layout.
	virtual void pack( void );
	void set_message( const JUTIL::String* message );

    // Resource loading.
	static bool precache( void );
	static void release( void );

protected:

	static JUI::FontInterface* font_;

private:

	JUI::WrappedText		*text_;
	JUI::Image				*image_;
	JUI::HorizontalLayout	*layout_;
	RoundedRectangle        *rounded_rect_;

};

#endif // NOTIFICATION_H