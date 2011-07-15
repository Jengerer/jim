#ifndef INDEX_SLOT_H
#define INDEX_SLOT_H

#include "slot.h"

class IndexSlot : public Slot
{

public:

	IndexSlot( unsigned int index );

	unsigned int GetIndex( void ) const;
	void			SetItem( Item* item );

private:

	void			SetIndex( unsigned int index );

private:

	unsigned int index_;

};

#endif // INDEX_SLOT_H