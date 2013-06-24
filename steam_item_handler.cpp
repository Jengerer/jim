#include "steam_item_handler.hpp"
#include "serialized_buffer.hpp"
#include "jui/application/error_stack.hpp"

/*
 * Item and selection handler constructor.
 */
SteamItemHandler::SteamItemHandler( void )
{
}

/*
 * Item and selection handler destructor.
 */
SteamItemHandler::~SteamItemHandler( void )
{
	close_interfaces();
}

/*
 * Update a single item.
 */
bool SteamItemHandler::update_item( const Item* item ) const
{
	GCSetItemPosition_t message;
	message.itemID = item->get_unique_id();
	message.position = item->get_inventory_flags();
	return send_message( 
		static_cast<uint32>(GCSetItemPosition_t::k_iMessage), 
		static_cast<void*>(&message),
		sizeof(message) );
}

/*
 * Delete a single item.
 */
bool SteamItemHandler::delete_item( const Item* item )
{
	GCDelete_t message;
	message.itemID = item->get_unique_id();
	return send_message(
		static_cast<uint32>(GCDelete_t::k_iMessage),
		&message,
		sizeof(message));
}

/*
 * Set number of items to be sent to craft message.
 */
bool SteamItemHandler::set_craft_size( unsigned int count )
{
	// Allocate space for base message and item IDs.
	unsigned int message_size = sizeof(GCCraft_t);
	message_size += sizeof(uint64) * count;
	if (!craft_buffer_.set_size( message_size )) {
		return false;
	}

	return true;
}

/*
 * Add an item to the item craft buffer.
 */
void SteamItemHandler::set_craft_item( unsigned int index, const Item* item )
{
	char* items_offset = craft_buffer_.get_array() + sizeof(GCCraft_t);
	uint64* items = reinterpret_cast<uint64*>(items_offset);
	items[index] = item->get_unique_id();
}

/*
 * Send the item IDs in the craft buffer array to be crafted.
 */
bool SteamItemHandler::craft_items( void )
{
	// Send message.
	if (!send_message( GCCraft_t::k_iMessage, craft_buffer_.get_array(), craft_buffer_.get_size() )) {
        return false;
    }

    return true;
}