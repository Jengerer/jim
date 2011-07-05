#include "backpack.h"

#include <json/json.h>

#include "protobuf/base_gcmessages.pb.h"
#include "protobuf/gcsdk_gcmessages.pb.h"
#include "protobuf/steammessages.pb.h"

// Navigation constants.
const int PAGE_CHANGE_DELAY	= 500;
const int BUTTON_SPACING	= 5;

// Spring constants.
const float SPRING_STRENGTH	= 0.1f;
const float SPRING_DAMPING	= 0.7f;

const int BACKPACK_PADDING		= 25;
const int BACKPACK_PADDING_TOP	= 50;

const int SLOT_SPACING			= 5;
const int PAGE_SPACING			= BACKPACK_PADDING * 2;

Backpack::Backpack( unsigned int inventorySize, unsigned int excludedSize ) : Inventory( inventorySize, excludedSize )
{
	SetLoaded( false );
}

/* void Backpack::Precache( DirectX *directX )
{
	// Get button textures.
	Texture *craftTexture = directX->GetTexture( "manager/gear" );
	Texture *equipTexture = directX->GetTexture( "manager/equip" );
	Texture *sortTexture = directX->GetTexture( "manager/sort" );

	// Create buttons.
	craftButton_ = Button::CreateIconLabelButton( craftTexture, "craft" );
	equipButton_ = Button::CreateIconLabelButton( equipTexture, "equip" );
	sortButton_ = Button::CreateIconLabelButton( sortTexture, "sort" );
	craftButton_->SetEnabled( false );
	equipButton_->SetEnabled( false );
	sortButton_->SetEnabled( false );
	
	// Add to layout.
	buttonLayout_->Add( craftButton_ );
	buttonLayout_->Add( equipButton_ );
	buttonLayout_->Add( sortButton_ );
	Pack();
} */

/* void Backpack::HandleMessage( int id, void *message, uint32 size )
{
	switch (id) {
	case SOMsgCreate_t::k_iMessage:
		{
			// Get object created.
			CMsgSOSingleObject deleteObj;
			deleteObj.ParseFromArray( message, size );

			// Get item from object.
			CSOEconItem econItem;
			econItem.ParseFromArray( deleteObj.object_data().data(), deleteObj.object_data().size() );

			// Now add item.
			Item* newItem = new Item(
				econItem.id(),
				econItem.def_index(),
				econItem.level(),
				(EItemQuality)econItem.quality(),
				econItem.quantity(),
				econItem.inventory() );

			// Add this item to excluded.
			inventory_->InsertItem( newItem );
			inventory_->UpdateExcluded();
			
			// Get the source.
			string source;
			if (econItem.origin() == 4) {
				source = "crafted";
			}
			else {
				source = "found";
			}

			// Display message.
			notifications_->AddNotification( "You have " + source + " a " + newItem->GetName() + ".", newItem->GetTexture() );

			break;
		}
		
	case SOMsgDeleted_t::k_iMessage:
		{
			// Get deleted message.
			CMsgSOSingleObject deleteObj;
			deleteObj.ParseFromArray( message, size );

			// Get ID of deleted item.
			CSOEconItem deletedItem;
			deletedItem.ParseFromArray( deleteObj.object_data().data(), deleteObj.object_data().size() );

			// Now remove from inventory.
			Item *targettedItem = inventory_->GetItemByUniqueId( deletedItem.id() );
			if (targettedItem != nullptr) {
				inventory_->RemoveItem( targettedItem->GetUniqueId() );
			}
			break;
		}

	case k_ESOMsg_UpdateMultiple:
		{
			CMsgSOMultipleObjects updateMsg;
			updateMsg.ParseFromArray( message, size );

			for (int i = 0; i < updateMsg.objects_size(); i++) {
				CMsgSOMultipleObjects::SingleObject singleObject = updateMsg.objects( i );
				if (singleObject.type_id() == 1) {
					CSOEconItem econItem;
					econItem.ParseFromArray( singleObject.object_data().data(), singleObject.object_data().size() );

					// Attempt to find the item.
					Item *item = inventory_->GetItemByUniqueId( econItem.id() );
					if (item == nullptr) {
						break;
					}

					// Place item into excluded, to be resolved later.
					item->SetFlags( econItem.inventory() );
					inventory_->ToExcluded( item );
				}
			}

			// Attempt to reposition excluded slots.
			inventory_->ResolveExcluded();
			break;
		}
	}
} */


/* void Backpack::LoadInventory( const string &jsonInventory )
{	
	// Begin inventory parsing.
	Json::Reader	reader;
	Json::Value		root;

	if (!reader.parse( jsonInventory, root, false ))
		throw Exception( "Failed to parse inventory JSON file." );

	// Result is root node.
	if (!root.isMember( "result" ))
		throw Exception( "Failed to parse player's items from Web API: no 'result' key received." );

	Json::Value result = root.get( "result", root );

	// Status is next node.
	if (!result.isMember( "status" ))
		throw Exception( "Failed to parse player's items from Web API: no 'status' key received." );
	int status = result.get( "status", root ).asInt();

	// Check validity.
	if (status == 15)
		throw Exception( "Failed to parse player's items from Web API: profile is private." );
	else if (status == 8)
		throw Exception( "Failed to parse player's items from Web API: invalid SteamID argument." );

	// Get the items object.
	if (!result.isMember( "items" ))
		throw Exception( "Failed to parse player's items from Web API: no 'items' key received." );
	Json::Value items = result.get( "items", root );
	
	// TODO: Check that 'item' exists in an empty inventory.
	// Get items array.
	if (!items.isMember( "item" ))
		throw Exception( "Failed to parse player's items from Web API: no 'item' key received." );
	Json::Value itemsArray = items.get( "item", root );
	
	// Keep track of attributes.
	SOMsgCacheSubscribed_Item_t item;
	ZeroMemory( &item, sizeof( SOMsgCacheSubscribed_Item_t ));

	for (Json::ValueIterator i = itemsArray.begin(); i != itemsArray.end(); i++) {
		Json::Value thisItem = *i;

		bool hasKeys = thisItem.isMember( "id" ) && 
			thisItem.isMember( "defindex" ) && 
			thisItem.isMember( "level" ) && 
			thisItem.isMember( "quality" ) && 
			thisItem.isMember( "inventory" ) &&
			thisItem.isMember( "quantity" );

		if (!hasKeys)
			throw Exception( "Failed to parse player's items from Web API: unexpected format for items received." );

		item.itemid			= thisItem.get( "id", root ).asUInt64();
		item.itemdefindex	= thisItem.get( "defindex", root ).asUInt();
		item.itemlevel		= thisItem.get( "level", root ).asUInt();
		item.itemquality	= thisItem.get( "quality", root ).asUInt();
		item.position		= thisItem.get( "inventory", root ).asUInt();

		// All attributes retrieved, make item.
		Item* newItem = new Item(
			item.itemid,
			item.itemdefindex,
			item.itemlevel,
			(EItemQuality)item.itemquality,
			item.quantity,
			item.position );

		// Add the item.
		inventory_->InsertItem( newItem );
	}

	inventory_->UpdateExcluded();
} */


SlotGridPages* Backpack::CreateInventoryView( unsigned int width, unsigned int height ) const
{
	SlotGridPages* inventoryView = new SlotGridPages( width, height,
		PAGE_SPACING, SLOT_SPACING );
	inventoryView->AddPages( inventory_ );
	return inventoryView;
}

SlotGridView* Backpack::CreateExcludedView() const
{
	SlotGridView* excludedView = new SlotGridView( excluded_->GetSlotCount(), SLOT_SPACING );
	excludedView->AddSlots( excluded_ );
	return excludedView;
}

bool Backpack::IsLoaded( void ) const
{
	return isLoaded_;
}

void Backpack::SetLoaded( bool isLoaded )
{
	isLoaded_ = isLoaded;
}

/*void Backpack::CraftSelected( void )
{
	// Number of items to craft.
	uint16 itemCount = selected_.size();

	if (itemCount > 0) {
		// Allocate and set.
		unsigned int messageSize = sizeof( struct GCCraft_t ) + sizeof( uint64 ) * itemCount;
		void* message = malloc( messageSize );
		memset( message, 0xff, messageSize );

		// Values to fill in.
		uint16 id = 1;

		// TODO: Warn user about untradable items.
		SerializedBuffer serialBuffer( message );
		serialBuffer.write( &id, sizeof( id ) );
		serialBuffer.push( 16 * sizeof( char ) );
		serialBuffer.push( sizeof( uint16 ) );
		serialBuffer.write( &itemCount, sizeof( itemCount ) );
	
		// Write all item IDs.
		for (int i = 0; i < itemCount; i++) {
			Slot *slot = selected_[i];
			Item *item = slot->GetItem();

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
}*/

/*void Backpack::UpdateItem( Item* item )
{
	// Generate message with new flags.
	// TODO: See if we can use protobufs here before old messages are deprecated.
	GCSetItemPosition_t message;
	memset( &message, 0xFF, sizeof( message ) );
	message.itemID = item->GetUniqueId();
	message.position = item->GetFlags();

	// Send it.
	SendMessage( GCSetItemPosition_t::k_iMessage, &message, sizeof( message ) );
}*/

/*void Backpack::MoveCamera( void )
{
	// Add elastic speed.
	float cameraDistance = cameraDest_ - pages_->GetLocalX();
	cameraSpeed_ += cameraDistance*SPRING_STRENGTH;
	cameraSpeed_ *= SPRING_DAMPING;

	// Stop the camera if slowing and near destination.
	float pagesX;
	float pagesY = pages_->GetLocalY();
	if (abs( cameraSpeed_ ) < 2.0f && abs( cameraDistance ) < 1.0f) {
		cameraSpeed_ = 0.0f;
		pagesX = cameraDest_;
	}
	else {
		pagesX = pages_->GetLocalX() + cameraSpeed_;
	}

	pages_->SetLocalPosition( pagesX, pagesY );
	backpackLayout_->UpdateChild( pages_ );
}*/