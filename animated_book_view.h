#ifndef ANIMATED_BOOK_VIEW_H
#define ANIMATED_BOOK_VIEW_H

#include "slot_book_view.h"

/*
 * Book view that bounces between pages.
 */
class AnimatedBookView : public SlotBookView
{

public:

	AnimatedBookView( const SlotBook* slot_book,
		unsigned int page_spacing,
		unsigned int slot_spacing );

	// Over-ride the view update.
	void update_view();

private:

	// View functions.
	float get_view_speed( void ) const;
	void set_view_speed( float view_speed );
	void set_view_position( float view_position );
	float get_view_position( void ) const;

private:

	// View members.
	float view_speed_;
	float view_position_;

};

#endif // ANIMATED_BOOK_VIEW_H