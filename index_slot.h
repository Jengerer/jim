#ifndef INDEX_SLOT_H
#define INDEX_SLOT_H

#include "slot.h"

class IndexSlot : public Slot
{

public:

	IndexSlot( unsigned int index );

	unsigned int get_index( void ) const;
	void			set_item( Item* item );

private:

	void			set_index( unsigned int index );

private:

	unsigned int index_;

};

#endif // INDEX_SLOT_H