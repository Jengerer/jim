#ifndef NOTICE_H
#define NOTICE_H

#include "idrawable.h"
#include "iprecachable.h"
#include "popup.h"
#include "rounded_container.h"
#include "wrapped_text.h"
#include "vertical_layout.h"

class Notice : public Popup
{

public:

	Notice( const std::string& message );
	virtual ~Notice( void );

	// Resizes the notification based on new text.
	virtual void	Pack( void );

	// Message handling.
	const std::string*	get_message( void ) const;
	void			SetMessage( const std::string& message );
	void			AppendMessage( const std::string& message );

	// Loading class-wide resources.
	static void		Precache( Graphics2D* graphics );
	static void		Release( void );

protected:

	// Class-wide texture and font.
	static Font*	font_;
	VerticalLayout*	content_;

private:

	RoundedRectangleContainer*	roundedContainer_;
	WrappedText*				text_;
	string						message_;

};

#endif // NOTICE_H