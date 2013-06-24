#ifndef STEAM_ITEM_HANDLER_H
#define STEAM_ITEM_HANDLER_H

#include "steam.hpp"
#include "steam_item_interface.hpp"

class SteamItemHandler : public Steam, public SteamItemInterface
{
public:

	SteamItemHandler( void );
	virtual ~SteamItemHandler( void );

	// Single item change functions.
	virtual bool update_item( const Item* item ) const;
	virtual bool delete_item( const Item* item );

	// Craft handling functions.
	virtual bool set_craft_size( unsigned int count );
	virtual void set_craft_item( unsigned int index, const Item* item );
	virtual bool craft_items( void );

private:

	// Crafting item buffer.
	JUTIL::ArrayBuilder<char> craft_buffer_;

};

#endif // STEAM_ITEM_HANDLER_HPP