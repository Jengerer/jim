#ifndef SLOT_HPP
#define SLOT_HPP

#include "item.hpp"

class Slot
{

public:

	Slot( void );

	// Item functions.
	bool has_item( void ) const;
	Item* get_item( void ) const;
	void set_item( Item* item );

    // Slot state functions.
    bool is_enabled( void ) const;
    void set_enabled( bool is_enabled );

private:

	Item* item_;
    bool is_enabled_;

};

#endif // SLOT_HPP