#include "steam_item_handler.h"
#include "serialized_buffer.h"

SteamItemHandler::SteamItemHandler()
{
	SetSelectMode( SELECT_MODE_SINGLE );
}

void SteamItemHandler::Select( SlotView* slotView )
{
	if (GetSelectMode() == SELECT_MODE_SINGLE) {
		DeselectAll();
	}

	selected_.insert( slotView );
	slotView->SetSelected( true );
}

void SteamItemHandler::Deselect( SlotView* slotView )
{
	selected_.erase( slotView );
	slotView->SetSelected( false );
}

void SteamItemHandler::ToggleSelect( SlotView* slotView )
{
	if (IsSelected( slotView )) {
		Deselect( slotView );
	}
	else {
		Select( slotView );
	}
}

void SteamItemHandler::DeselectAll()
{
	for (auto i = selected_.begin(); i != selected_.end(); i = selected_.erase( i )) {
		SlotView* slotView = *i;
		slotView->SetSelected( false );
	}
}

bool SteamItemHandler::IsSelected( SlotView* slotView ) const
{
	return selected_.find( slotView ) != selected_.end();
}

size_t SteamItemHandler::GetSelectedCount() const
{
	return selected_.size();
}

void SteamItemHandler::SetSelectMode( SelectMode selectMode )
{
	selectMode_ = selectMode;
}

SelectMode SteamItemHandler::GetSelectMode() const
{
	return selectMode_;
}

void SteamItemHandler::UpdateItem( const Item* item ) const
{
	GCSetItemPosition_t message;
	memset( &message, 0xFF, sizeof( message ) );
	message.itemID = item->GetUniqueId();
	message.position = item->GetFlags();

	// Send it.
	SendMessage( 
		static_cast<uint32>(GCSetItemPosition_t::k_iMessage), 
		(void*)&message,
		sizeof( message ) );
}

void SteamItemHandler::CraftSelected()
{
	// Number of items to craft.
	uint16 itemCount = GetSelectedCount();
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
			Item* item = slotView->GetSlot()->GetItem();

			// Get ID and write.
			uint64 itemId = item->GetUniqueId();
			serialBuffer.write( &itemId, sizeof( itemId ) );
		}

		// Deselect everything.
		DeselectAll();

		// Send message.
		SendMessage( GCCraft_t::k_iMessage, message, messageSize );
		free( message );
	}
}

void SteamItemHandler::EquipSelected( EClassEquip whichClass, bool setEquip )
{
	// To be implemented.
}

bool SteamItemHandler::CanEquipSelected() const
{
	if (GetSelectedCount() == 1) {
		SlotView* selected = *selected_.begin();
		Item* item = selected->GetSlot()->GetItem();
		return item->GetEquipClassCount() != 0;
	}

	return false;
}