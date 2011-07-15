#ifndef SLOT_H
#define SLOT_H

#include "item.h"

class Slot
{
public:

	Slot();

	// Item functions.
	bool			HasItem( void ) const;
	Item*			GetItem( void ) const;
	virtual void	SetItem( Item* item );

private:

	Item*			item_;

};

#endif // SLOT_H