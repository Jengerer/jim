#ifndef NOTICE_H
#define NOTICE_H

#include <jui/wrapped_text.h>
#include <jui/vertical_layout.h>

#include "popup.h"
#include "rounded_rectangle_container.h"

class Notice : public Popup
{

public:

	Notice( const std::string& message );
	virtual ~Notice( void );

	// Resizes the notification based on new text.
	virtual void	pack( void );

	// Message handling.
	const std::string*	get_message( void ) const;
	void			set_message( const std::string& message );
	void			append_message( const std::string& message );

	// Loading class-wide resources.
	static void		precache( Graphics2D* graphics );
	static void		release( void );

protected:

	// Class-wide texture and font.
	static IFont*	font_;
	VerticalLayout*	content_;

private:

	RoundedRectangleContainer*	roundedContainer_;
	WrappedText*				text_;
	string						message_;

};

#endif // NOTICE_H