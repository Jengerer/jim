#ifndef NOTICE_H
#define NOTICE_H

#include <jui/gfx/wrapped_text.hpp>
#include <jui/layout/vertical_layout.hpp>

#include "popup.hpp"
#include "rounded_rectangle_container.hpp"

class Notice : public Popup
{

public:

    static Notice* create( const JUTIL::ConstantString& message );
	virtual ~Notice( void );

	// Resizes the notification based on new text.
	virtual void pack( void );

	// Message handling.
	const JUTIL::String* get_message( void ) const;
    bool set_message( const JUTIL::ConstantString& message );
	bool append_message( const JUTIL::ConstantString& message );

	// Loading class-wide resources.
	static bool precache( JUI::Graphics2D* graphics );
	static void release( void );

protected:

    // Only self and children can construct safely.
    Notice( RoundedRectangleContainer* rounded_container,
        JUI::WrappedText* text,
        JUTIL::StringBuilder* builder );

protected:

	// Class-wide texture and font.
	static JUI::FontInterface* font_;
	JUI::VerticalLayout* content_;

private:

	RoundedRectangleContainer* rounded_container_;
	JUI::WrappedText* text_;
	JUTIL::String message_;

};

#endif // NOTICE_H