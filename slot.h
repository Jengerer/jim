#ifndef SLOT_H
#define SLOT_H

#include "item.h"

class Slot
{
public:

	Slot();

	// Item functions.
	bool			has_item( void ) const;
	Item*			get_item( void ) const;
	virtual void	set_item( Item* item );

private:

	Item*			item_;

};

#endif // SLOT_H