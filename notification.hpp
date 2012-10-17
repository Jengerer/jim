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

    // Initialization and creation.
	Notification( void );
	virtual ~Notification( void );
    bool initialize( const JUI::Texture *texture );

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
	JUI::HorizontalLayout	*layout_;

};

#endif // NOTIFICATION_H