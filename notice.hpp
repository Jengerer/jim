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
    virtual bool initialize( const JUTIL::ConstantString& message );
	virtual void pack( void );

	// Message handling.
	const JUTIL::String* get_message( void ) const;
    bool set_message( const JUTIL::ConstantString& message );
	bool append_message( const JUTIL::ConstantString& message );

	// Loading class-wide resources.
	static bool precache( JUI::Graphics2D* graphics );
	static void release( void );    

protected:

	// Class-wide texture and font.
	static JUI::FontInterface* font_;

    // Class-family layout member.
    JUI::VerticalLayout* content_;

private:

    // Notice layout members.
	RoundedRectangleContainer* rounded_container_;
	JUI::WrappedText* text_;
	JUTIL::String message_;

};

#endif // NOTICE_HPP