#ifndef SLOT_H
#define SLOT_H

#include "item.h"

class Slot
{

public:

	Slot( unsigned int index );

	// Item functions.
	bool			has_item( void ) const;
	Item*			get_item( void ) const;
	void			set_item( Item* item );
	void			remove_item( void );

	// Index functions.
	unsigned int	get_index( void ) const;

private:

	// Index functions.
	void			set_index( unsigned int index );

private:

	Item*			item_;
	unsigned int	index_;

};

#endif // SLOT_H