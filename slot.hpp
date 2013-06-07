#ifndef SLOT_HPP
#define SLOT_HPP

#include "item.hpp"

class Slot
{

public:

	Slot( void );

	// Item functions.
	bool			has_item( void ) const;
	Item*			get_item( void ) const;
	void			set_item( Item* item );

private:

	Item*			item_;

};

#endif // SLOT_HPP