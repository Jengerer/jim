#ifndef NOTICE_HPP
#define NOTICE_HPP

#include <jui/gfx/wrapped_text.hpp>
#include <jui/layout/vertical_layout.hpp>

#include "popup.hpp"
#include "rounded_rectangle_container.hpp"

class Notice : public Popup
{

public:

    // Only self and children can construct safely.
    Notice( void );
	virtual ~Notice( void );

	// Layout management.
    virtual bool initialize( const JUTIL::String* message );

	// Message handling.
    void set_message( const JUTIL::String* message );

	// Loading class-wide resources.
	static bool precache( JUI::Graphics2D* graphics );

protected:

	// Class-wide texture and font.
	static JUI::FontInterface* font_;

private:

    // Notice layout members.
	JUI::WrappedText* text_;

};

#endif // NOTICE_HPP