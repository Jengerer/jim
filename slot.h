#ifndef SLOT_H
#define SLOT_H

#include "item.h"

class Slot
{
public:

	Slot( int index );

	// Item functions.
	bool		HasItem( void ) const;
	Item*		GetItem( void ) const;
	void		SetItem( Item* item );

	int			GetIndex( void ) const;
	void		SetIndex( int index );

private:

	// Item stored here.
	Item*				item_;

	// Information about slot.
	int					index_;
	ESlotGroup			group_;
	ESelectType			selectType_;

};

#endif // SLOT_H