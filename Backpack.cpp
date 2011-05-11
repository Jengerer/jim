#include "Backpack.h"

#include "protobuf/base_gcmessages.pb.h"
#include "protobuf/gcsdk_gcmessages.pb.h"
#include "protobuf/steammessages.pb.h"

// Navigation constants.
const int	PAGE_CHANGE_DELAY	= 500;

// Spring constants.
const float SPRING_STRENGTH		= 0.1f;
const float SPRING_DAMPING		= 0.7f;

Backpack::Backpack(
	float x, float y,
	Container* parent ): Container( x, y )
{
	// No inventory until size set.
	inventory_ = nullptr;
	
	// Inventory formatting pointers.
	pages_ = new HorizontalLayout();
	excluded_ = new HorizontalLayout();
	Add( pages_ );
	Add( excluded_ );

	// Slot state information.
	dragged_ = nullptr;
	hovered_ = nullptr;

	// Starting attributes.
	isLoaded_ = false;
	cameraSpeed_ = 0;
	page_ = 1;

	// Move to start.
	SetPosition( x, y );
	SetSize( parent->GetWidth(), parent->GetHeight() );
}

Backpack::~Backpack()
{
	removeSlots();
	closeInterfaces();

	if (inventory_ != nullptr) {
		delete inventory_;
		inventory_ = nullptr;
	}
}

void Backpack::openInterfaces()
{
	Steam::openInterfaces();
}

void Backpack::closeInterfaces()
{
	Steam::closeInterfaces();
}

void Backpack::handleCallback( int id, void *callback )
{
}

void Backpack::handleMessage( int id, void *message, uint32 size )
{
	switch (id) {
	case SOMsgCacheSubscribed_t::k_iMessage:
		{
			if ( isLoaded() ) {
				return;
			}

			CMsgSOCacheSubscribed subscribedMsg;
			subscribedMsg.ParseFromArray( message, size );
								
			// TODO: Check for other users' backpack.
			for (int i = 0; i < subscribedMsg.objects_size(); i++) {
				CMsgSOCacheSubscribed_SubscribedType subscribedType = subscribedMsg.objects( i );
				switch ( subscribedType.type_id() ) {
				case 1:
					{
						if ( getSteamId() == subscribedMsg.owner() ) {
							for (int i = 0; i < subscribedType.object_data_size(); i++) {
								CSOEconItem econItem;
								econItem.ParseFromArray( subscribedType.object_data( i ).data(), subscribedType.object_data( i ).size() );
								Item *item = new Item(
									econItem.id(),
									econItem.def_index(),
									econItem.level(),
									(EItemQuality)econItem.quality(),
									econItem.quantity(),
									econItem.inventory() );
								inventory_->AddItem( item );
							}
						}

						break;
					}
				case 7:
					{
						if ( subscribedMsg.owner() == getSteamId() ) {
							for (int i = 0; i < subscribedType.object_data_size(); i++) {
								CSOEconGameAccountClient gameAccountClient;
								gameAccountClient.ParseFromArray( subscribedType.object_data( i ).data(), subscribedType.object_data( i ).size() );
								inventory_->AddSlots( gameAccountClient.additional_backpack_slots() );
							}
						}

						break;
					}
				}

			}

			setLoaded( true );
			inventory_->UpdateExcluded();
			break;
		}

	case SOMsgCreate_t::k_iMessage:
		{
			SOMsgCreate_t *msgCreate = (SOMsgCreate_t*)message;
			SOMsgCacheSubscribed_Item_t* craftedItem = &msgCreate->item;

			//Make sure it's a valid item.
			if (msgCreate->unknown == 5) {
				break;
			}

			Item* newItem = new Item(
				craftedItem->itemid,
				craftedItem->itemdefindex,
				craftedItem->itemlevel,
				(EItemQuality)craftedItem->itemquality,
				craftedItem->quantity,
				craftedItem->position);

			// Add this item to excluded.
			inventory_->AddItem( newItem );
			inventory_->UpdateExcluded();
			break;
		}
		
	case SOMsgDeleted_t::k_iMessage:
		{
			SOMsgDeleted_t *deleteMsg = (SOMsgDeleted_t*)message;
			Item *target = inventory_->GetItemByUniqueId( deleteMsg->itemid );
			if (target != nullptr) {
				inventory_->RemoveItem( target );
				inventory_->UpdateExcluded();

				if ((hovered_ != nullptr) && (target == hovered_->GetItem())) {
					hovered_ = nullptr;
				}
			}
			break;
		}
	}
}

void Backpack::createInventory( int width, int height, int pages, int excludedSize )
{
	inventory_ = new Inventory( width, height, pages, excludedSize );
	formatInventory();
}

void Backpack::loadInventory( const string &jsonInventory )
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
		inventory_->AddItem( newItem );
	}
}

void Backpack::formatInventory()
{
	// Add slots to layout.
	pages_->SetParent( this );
	pages_->SetPosition( BACKPACK_PADDING, BACKPACK_PADDING_TOP );
	pages_->SetSpacing( PAGE_SPACING );

	int pages = inventory_->GetPageCount();
	int pageWidth = inventory_->GetWidth();
	int pageHeight = inventory_->GetHeight();
	for (int i = 0; i < pages; i++) {
		// Add a column per page.
		HorizontalLayout *pageColumns = new HorizontalLayout();
		pageColumns->SetParent( this );
		pageColumns->SetSpacing( SLOT_SPACING );

		// Split the page into two.
		for (int j = 0; j < 2; j++) {
			// Add a row of slots per column.
			for (int x = 0; x < (pageWidth >> 1); x++) {
				VerticalLayout *rows = new VerticalLayout();
				rows->SetSpacing( SLOT_SPACING );

				// Add a slot per row.
				for (int y = 0; y < pageHeight; y++) {
					// Simplified formula for index.
					int index = x + j * (pageWidth >> 1) + pageWidth * (i * pageHeight + y);
					Slot* slot = inventory_->GetInventorySlot( index );
					rows->Add( slot );
				}

				rows->Pack();
				pageColumns->Add( rows );
			}
		}

		pageColumns->Pack();
		pages_->Add( pageColumns );
	}

	pages_->Pack();

	// Position and add excluded.
	excluded_->SetPosition( GetX() + BACKPACK_PADDING, GetHeight() - SLOT_HEIGHT - BACKPACK_PADDING );
	excluded_->SetSpacing( SLOT_SPACING );

	int length = inventory_->GetExcludedSize();
	for (int i = 0; i < length; i++) {
		Slot* slot = inventory_->GetExcludedSlot( i );
		excluded_->Add( slot );
	}

	excluded_->Pack();

	// Set primary camera target.
	updateTarget();
	cameraX_ = cameraDest_;
}

bool Backpack::isLoaded() const
{
	return isLoaded_;
}

void Backpack::setLoaded( bool isLoaded )
{
	isLoaded_ = isLoaded;
}

void Backpack::moveItem( Slot *source, Slot *destination ) {
	Item* sourceItem = source->GetItem();
	Item* destItem = destination->GetItem();

	// Update items.
	inventory_->MoveItem( source, destination );
	updateItem( sourceItem ); // Definitely not null.
	if (destItem != nullptr) {
		updateItem( destItem );
	}
}

void Backpack::UpdatePosition()
{
	// Position all slots.
	if (pages_ != nullptr) {
		pages_->SetX( GetX() - cameraX_ );
		pages_->UpdatePosition();
	}
}

bool Backpack::OnMouseMoved( Mouse *mouse )
{
	// Mouse moved.
	if (selected_.size() == 1 && dragged_ != nullptr) {
		Slot* slot = selected_[0];
		slot->UpdatePosition();

		// No longer hovering.
		SetHovering( nullptr );

		// Change page if at edges.
		int time = GetTickCount();
		if (time > pageDelay_) {
			if (slot->GetX() == (GetWidth() - slot->GetWidth())) {
				nextPage();
				pageDelay_ = time + PAGE_CHANGE_DELAY;
			}
			else if (slot->GetX() == 0) {
				prevPage();
				pageDelay_ = time + PAGE_CHANGE_DELAY;
			}
		}

		return true;
	}

	// Go through visible pages.
	deque<Component*> *pageComponents = pages_->GetChildren();
	for each (Container *page in *pageComponents) {
		if (IsVisible( page )) {
			// Go through visible page columns.
			deque<Component*> *columns = page->GetChildren();
			for each (Container *column in *columns) {
				if (IsVisible( column )) {
					// Go through slots in column.
					deque<Component*> *slots = column->GetChildren();
					for each (Component *rowSlot in *slots) {
						Slot *slot = dynamic_cast<Slot*>(rowSlot);
						if (mouse->isTouching( slot )) {
							SetHovering( slot->HasItem() ? slot : nullptr );
							return true;
						}
					}
				}
			}
		}
	}

	// Check excluded.
	int length = inventory_->GetExcludedSize();
	for (int i = 0; i < length; i++) {
		Slot *slot = inventory_->GetExcludedSlot( i );
		if (mouse->isTouching( slot )) {
			SetHovering( slot->HasItem() ? slot : nullptr );
			return true;
		}
	}

	SetHovering( nullptr );
	return false;
}

bool Backpack::OnLeftClicked( Mouse *mouse )
{
	// Go through visible pages.
	deque<Component*> *pageComponents = pages_->GetChildren();
	for each (Container *page in *pageComponents) {
		if (IsVisible( page )) {
			// Go through visible page columns.
			deque<Component*> *columns = page->GetChildren();
			for each (Container *column in *columns) {
				if (WithinBounds( column )) {
					// Go through slots in column.
					deque<Component*> *slots = column->GetChildren();
					for each (Component *rowSlot in *slots) {
						Slot *slot = dynamic_cast<Slot*>(rowSlot);
						if (mouse->isTouching( slot )) {
							if (dragged_ == nullptr) {
								slotGrabbed( mouse, slot );
							}
							else {
								slotReleased( slot );
							}
							return true;
						}
					}
				}
			}
		}
	}

	// Check excluded.
	int length = inventory_->GetExcludedSize();
	for (int i = 0; i < length; i++) {
		Slot *slot = inventory_->GetExcludedSlot( i );
		if (mouse->isTouching( slot )) {
			if (dragged_ == nullptr) {
				slotGrabbed( mouse, slot );
			}
			else {
				slotReleased( slot );
			}
			return true;
		}
	}

	// Nothing hit, deselect all.
	if (selectMode_ != SELECT_MODE_MULTIPLE) {
		deselectAll();
	}

	return false;
}

bool Backpack::OnLeftReleased( Mouse *mouse )
{
	if (dragged_ != nullptr) {
		// Move item back to slot.
		Slot* selectedSlot = selected_[0];
		dragged_->SetItem( selectedSlot->GetItem() );

		// Remove dummy slot.
		selectedSlot->OnRelease();
		Remove( selectedSlot );
		delete selectedSlot;
		selected_.clear();

		// Go through visible pages.
		deque<Component*> *pageComponents = pages_->GetChildren();
		for each (Container *page in *pageComponents) {
			if (IsVisible( page )) {
				// Go through visible page columns.
				deque<Component*> *columns = page->GetChildren();
				for each (Container *column in *columns) {
					if (IsVisible( column )) {
						// Go through slots in column.
						deque<Component*> *slots = column->GetChildren();
						for each (Component *rowSlot in *slots) {
							Slot *slot = dynamic_cast<Slot*>(rowSlot);
							if (mouse->isTouching( slot )) {
								slotReleased( slot );
								dragged_ = nullptr;
								return true;
							}
						}
					}
				}
			}
		}

		// Dragged was not moved.
		select( dragged_, SELECT_TYPE_NORMAL );
		dragged_ = nullptr;
		return true;
	}

	return false;
}

bool Backpack::OnRightClicked( Mouse *mouse )
{
	return false;
}

bool Backpack::OnRightReleased( Mouse *mouse )
{
	return false;
}

void Backpack::slotGrabbed( Mouse *mouse, Slot *slot )
{
	// Clear selected.
	if (selectMode_ == SELECT_MODE_SINGLE) {
		deselectAll();
		if (slot->GetItem() != nullptr) {
			dragged_ = slot;

			// Create a dummy slot to drag the item.
			Slot* dragging = new Slot( *slot );
			dragging->SetIndex( slot->GetIndex() );
			dragging->SetGroup( slot->GetGroup() );
			dragging->SetItem( slot->GetItem() );
			dragging->SetParent( this );
			Add( dragging );
		
			// Remove item from slot.
			slot->SetItem( nullptr );

			// Start dragging.
			dragging->OnDrag( mouse );
			select( dragging, SELECT_TYPE_DRAG );
		}
	}
	else {
		if (slot->GetItem() != 0) {
			switch (slot->GetSelectType()) {
			case SELECT_TYPE_NONE:
				select( slot, SELECT_TYPE_NORMAL );
				break;
			default:
				deselect( slot );
				break;
			}
		}
	}
}

void Backpack::slotReleased( Slot *slot )
{
	// Skip if returning or excluded.
	if (slot != dragged_) {
		if (slot->GetItem() == nullptr || dragged_->GetGroup() == GROUP_INVENTORY) {
			// Move to slot if able.
			moveItem( dragged_, slot );
			selected_.push_back( slot );
			slot->SetSelectType( SELECT_TYPE_NORMAL );
			slot->UpdatePosition();
		}
	}
	else {
		// Set to regular select.
		select( dragged_, SELECT_TYPE_NORMAL );
	}
}

void Backpack::removeSlots() {
	if (pages_ != nullptr) {
		deque<Component*> *pageComponents = pages_->GetChildren();
		for each (Container *page in *pageComponents) {
			deque<Component*> *columns = page->GetChildren();
			for each (Container *column in *columns) {
				column->RemoveAll();
			}
		}
	}

	if (excluded_ != nullptr) {
		excluded_->RemoveAll();
	}
}

void Backpack::equipItem( Item *item, const string& className ) {
	EClassEquip equipClass;
	if (className == "scout") {
		equipClass = CLASS_SCOUT;
	}
	else if (className == "soldier") {
		equipClass = CLASS_SOLDIER;
	}
	else if (className == "pyro") {
		equipClass = CLASS_PYRO;
	}
	else if (className == "demoman") {
		equipClass = CLASS_DEMOMAN;
	}
	else if (className == "heavy") {
		equipClass = CLASS_HEAVY;
	}
	else if (className == "engineer") {
		equipClass = CLASS_ENGINEER;
	}
	else if (className == "medic") {
		equipClass = CLASS_MEDIC;
	}
	else if (className == "sniper") {
		equipClass = CLASS_SNIPER;
	}
	else if (className == "spy") {
		equipClass = CLASS_SPY;
	}
	else {
		throw Exception( "Attempting to equip to unknown class." );
	}

	if (item->IsEquipped( equipClass )) {
		item->SetEquip( equipClass, false );
	}
	else {
		unequipItems( equipClass, item->GetEquipSlot() );
		item->SetEquip( equipClass, true );
	}

	updateItem( item );
}

void Backpack::unequipItems( EClassEquip equipClass, const string& slot ) {
	const itemVector* inventoryItems = inventory_->GetInventoryItems();
	for each(Item *item in *inventoryItems) {
		if (item->IsEquipped( equipClass )) {
			if (item->GetEquipSlot() == slot) {
				item->SetEquip( equipClass , false );
				updateItem( item );
				break;
			}
		}
	}

	const itemVector* excludedItems = inventory_->GetExcludedItems();
	for each (Item *item in *excludedItems) {
		if (item->IsEquipped( equipClass )) {
			if (item->GetEquipSlot() == slot) {
				item->SetEquip( equipClass , false );
				updateItem( item );
				break;
			}
		}
	}
}

void Backpack::select( Slot* slot, ESelectType selectType )
{
	slot->SetSelectType( selectType );
	selected_.push_back( slot );
}

void Backpack::deselect( Slot* slot )
{
	slot->SetSelectType( SELECT_TYPE_NONE );

	// Remove from selected.
	slotVector::iterator i;
	for (i = selected_.begin(); i != selected_.end(); i++) {
		Slot *current = *i;
		if (current == slot) {
			selected_.erase( i );
			break;
		}
	}
}

void Backpack::deselectAll()
{
	// Deselect all.
	while (!selected_.empty()) {
		Slot *slot = selected_.back();
		deselect( slot );
	}
}

void Backpack::craftSelected()
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
		deselectAll();

		// Send message.
		sendMessage( GCCraft_t::k_iMessage, message, messageSize );
		free( message );
	}
}

void Backpack::updateItem( Item* item )
{
	// Generate message with new flags.
	GCSetItemPosition_t message;
	memset( &message, 0xff, sizeof( message ) );

	message.itemID = item->GetUniqueId();
	message.position = item->GetFlags();

	// Send it.
	sendMessage( GCSetItemPosition_t::k_iMessage, &message, sizeof( message ) );
}

slotVector* Backpack::getSelected()
{
	return &selected_;
}

void Backpack::setSelectMode( ESelectMode selectMode)
{
	selectMode_ = selectMode;
}

bool Backpack::IsHovering( void ) const
{
	return (hovered_ != nullptr) && (hovered_->GetItem() != nullptr);
}

const Slot* Backpack::GetHovering( void ) const
{
	return hovered_;
}

void Backpack::SetHovering( const Slot *slot )
{
	hovered_ = slot;
}

void Backpack::updateTarget()
{
	deque<Component*>* pageColumns = pages_->GetChildren();
	Component *cameraTarget = pageColumns->at( page_ - 1 );
	cameraDest_ = cameraTarget->GetX() - pages_->GetX() - BACKPACK_PADDING;
}

void Backpack::nextPage()
{
	if (page_ < inventory_->GetPageCount()) {
		page_++;
	}

	updateTarget();
}

void Backpack::prevPage()
{
	if (page_ > 1) {
		page_--;
	}

	updateTarget();
}

void Backpack::handleCamera()
{
	moveCamera();
	UpdatePosition();
}

void Backpack::moveCamera()
{
	// Add elastic speed.
	float cameraDistance = cameraDest_ - cameraX_;
	cameraSpeed_ += cameraDistance*SPRING_STRENGTH;
	cameraSpeed_ *= SPRING_DAMPING;

	// Stop if slowing.
	if (abs(cameraSpeed_) < 2 && abs(cameraDistance) < 1) {
		cameraSpeed_ = 0;
		cameraX_ = cameraDest_;
	}

	// Now propel.
	cameraX_ += cameraSpeed_;
}
