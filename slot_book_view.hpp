#ifndef SLOT_BOOK_VIEW_HPP
#define SLOT_BOOK_VIEW_HPP

#include "slot_grid_view.hpp"

/*
 * Class to control viewing a slot array a page at a time.
 */
class SlotBookView : public SlotGridView
{

public:

	SlotBookView( SlotArrayInterface* slot_array, unsigned int page_width, unsigned int page_height );
	virtual ~SlotBookView( void );

	// Page switching functions.
	bool set_active_page( unsigned int page );
	unsigned int get_active_page( void ) const;
	unsigned int get_page_count( void ) const;

    // UI handling.
    const SlotView* get_slot_view( unsigned int index ) const;

	// Slot update event over-ride.
	virtual bool on_slot_updated( unsigned int index, const Slot* slot );

protected:

    // Over-ride touching index.
    virtual bool get_touching_index( const JUI::Mouse* mouse, unsigned int* index ) const;

private:

	// Page parameters.
	unsigned int active_page_;
	unsigned int page_width_;
    unsigned int page_height_;

};

#endif // SLOT_BOOK_HPP