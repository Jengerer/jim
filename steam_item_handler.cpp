#include "steam_item_handler.h"
#include "serialized_buffer.h"

SteamItemHandler::SteamItemHandler()
{
	set_select_mode( SELECT_MODE_SINGLE );
}

SteamItemHandler::~SteamItemHandler()
{
	close_interfaces();
}

void SteamItemHandler::select( SlotView* slotView )
{
	if (get_select_mode() == SELECT_MODE_SINGLE) {
		deselect_all();
	}

	selected_.insert( slotView );
	slotView->set_selected( true );
}

void SteamItemHandler::deselect( SlotView* slotView )
{
	selected_.erase( slotView );
	slotView->set_selected( false );
}

void SteamItemHandler::toggle_select( SlotView* slotView )
{
	if (is_selected( slotView )) {
		deselect( slotView );
	}
	else {
		select( slotView );
	}
}

void SteamItemHandler::deselect_all()
{
	for (auto i = selected_.begin(); i != selected_.end(); i = selected_.erase( i )) {
		SlotView* slotView = *i;
		slotView->set_selected( false );
	}
}

bool SteamItemHandler::is_selected( SlotView* slotView ) const
{
	return selected_.find( slotView ) != selected_.end();
}

size_t SteamItemHandler::get_selected_count() const
{
	return selected_.size();
}

void SteamItemHandler::set_select_mode( SelectMode selectMode )
{
	selectMode_ = selectMode;
}

SelectMode SteamItemHandler::get_select_mode() const
{
	return selectMode_;
}

void SteamItemHandler::update_item( const Item* item ) const
{
	GCset_itemPosition_t message;
	memset( &message, 0xFF, sizeof( message ) );
	message.itemID = item->get_unique_id();
	message.position = item->get_flags();

	// Send it.
	send_message( 
		static_cast<uint32>(GCset_itemPosition_t::k_iMessage), 
		(void*)&message,
		sizeof( message ) );
}

bool SteamItemHandler::is_selected_tradable( void ) const
{
	for (auto i = selected_.begin(); i != selected_.end(); ++i) {
		Slot* slot = (*i)->get_slot();
		if (slot->has_item()) {
			Item* item = slot->get_item();

			// Check that the item can be traded.
			if (!item->is_tradable()) {
				return false;
			}
		}
	}

	return true;
}

void SteamItemHandler::craft_selected()
{
	// Number of items to craft.
	uint16 itemCount = get_selected_count();
	if (itemCount != 0) {
		// Allocate and set.
		unsigned int messageSize = sizeof( struct GCCraft_t ) + sizeof( uint64 ) * itemCount;
		void* message = malloc( messageSize );
		memset( message, 0xff, messageSize );

		// Values to fill in.
		uint16 id = 1;

		// TODO: Warn user about untradable items before we call this function.
		SerializedBuffer serialBuffer( message );
		serialBuffer.write( &id, sizeof( id ) );
		serialBuffer.push( 16 * sizeof( char ) );
		serialBuffer.push( sizeof( uint16 ) );
		serialBuffer.write( &itemCount, sizeof( itemCount ) );
	
		// Write all item IDs.
		for (auto i = selected_.begin(); i != selected_.end(); ++i) {
			SlotView* slotView = *i;
			Item* item = slotView->get_slot()->get_item();

			// Get ID and write.
			uint64 itemId = item->get_unique_id();
			serialBuffer.write( &itemId, sizeof( itemId ) );
		}

		// Deselect everything.
		deselect_all();

		// Send message.
		send_message( GCCraft_t::k_iMessage, message, messageSize );
		free( message );
	}
}

void SteamItemHandler::equip_selected( EClassEquip whichClass, bool setEquip )
{
	// To be implemented.
}

bool SteamItemHandler::can_equip_selected() const
{
	if (get_selected_count() == 1) {
		SlotView* selected = *selected_.begin();
		Item* item = selected->get_slot()->get_item();
		return item->get_equip_class_count() != 0;
	}

	return false;
}