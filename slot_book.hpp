#ifndef SLOT_BOOK_H
#define SLOT_BOOK_H

#include <containers/vector.hpp>
#include "slot_array.hpp"
#include "slot_container.hpp"

/*
 * Class for maintaining a set of slot pages.
 */
class SlotBook : public SlotContainer
{

public:

	SlotBook( unsigned int width, unsigned int height );
	virtual ~SlotBook( void );

	// Page management.
	const SlotArray* add_page( void );
	bool add_pages( unsigned int pages );
	void remove_page( void );
	const SlotArray* get_page( unsigned int page ) const;

	// Page capacity functions.
	unsigned int get_page_count( void ) const;
	unsigned int get_page_width( void ) const;
	unsigned int get_page_height( void ) const;
	unsigned int get_page_size( void ) const;
	
	// Slot functions.
	unsigned int get_slot_count( void ) const;
	Slot* get_slot( unsigned int index ) const;

private:

	// Page functions.
	void set_page_width( unsigned int width );
	void set_page_height( unsigned int height );
	void destroy_pages( void );

private:

	JUTIL::Vector<SlotArray*> pages_;
	unsigned width_;
	unsigned height_;

};

#endif // SLOT_BOOK_H