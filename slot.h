#ifndef SLOT_H
#define SLOT_H

#include "item.h"

class Slot
{

public:

	Slot( void );

	// Item functions.
	bool			has_item( void ) const;
	Item*			get_item( void ) const;
	void			set_item( Item* item );
	void			remove_item( void );

private:

	Item*			item_;

};

#endif // SLOT_H