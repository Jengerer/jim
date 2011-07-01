#ifndef SLOT_H
#define SLOT_H

#include "item.h"

class Slot
{
public:

	Slot( int index );

	// Item functions.
	bool	HasItem( void ) const;
	Item*	GetItem( void ) const;
	void	SetItem( Item* item );

	// Slot indexing.
	unsigned int	GetIndex( void ) const;

private:

	void			SetIndex( unsigned int index );

private:

	// Item stored here.
	Item*			item_;
	unsigned int	index_;

};

#endif // SLOT_H