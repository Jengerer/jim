#include "Backpack.h"

#include "protobuf/base_gcmessages.pb.h"
#include "protobuf/gcsdk_gcmessages.pb.h"
#include "protobuf/steammessages.pb.h"

// Navigation constants.
const int	PAGE_CHANGE_DELAY	= 500;
const int	BUTTON_SPACING		= 5;

// Spring constants.
const float SPRING_STRENGTH		= 0.1f;
const float SPRING_DAMPING		= 0.7f;

// Inventory attributes.
const int PAGE_WIDTH			= 10;
const int PAGE_HEIGHT			= 5;
const int PAGE_COUNT			= 6;
const int EXCLUDED_SIZE			= 5;

Backpack::Backpack(
	float x, float y,
	Container* parent ): Container( x, y )
{
	// No inventory until size set.
	inventory_ = nullptr;

	// Default attributes.
	SetLoaded( false );
	cameraSpeed_ = 0.0f;
	page_ = 1;

	// Create overall layout.
	backpackLayout_ = new VerticalLayout();
	backpackLayout_->SetSpacing( 10 );
	backpackLayout_->SetAlignType( ALIGN_LEFT );
	backpackLayout_->SetLocalPosition( BACKPACK_PADDING, BACKPACK_PADDING_TOP );
	Add( backpackLayout_ );

	// Generate default inventory size.
	CreateInventory( PAGE_WIDTH, PAGE_HEIGHT, PAGE_COUNT, EXCLUDED_SIZE );

	// Lay out buttons.
	buttonLayout_ = new HorizontalLayout();
	buttonLayout_->SetSpacing( BUTTON_SPACING );
	craftButton_ = LabelButton::Create( "craft" );
	equipButton_ = LabelButton::Create( "equip" );
	sortButton_ = LabelButton::Create( "sort" );
	buttonLayout_->Add( craftButton_ );
	buttonLayout_->Add( equipButton_ );
	buttonLayout_->Add( sortButton_ );
	buttonLayout_->Pack();

	// Add all and pack.
	backpackLayout_->Add( pages_ );
	backpackLayout_->Add( buttonLayout_ );
	backpackLayout_->Add( excluded_ );
	Pack();

	// Slot state information.
	dragged_ = nullptr;
	SetHovering( nullptr );

	// Set default notification queue.
	SetNotificationQueue( nullptr );

	// Move to start.
	SetSize( parent->GetWidth(), parent->GetHeight() );
}

Backpack::~Backpack()
{
	RemoveSlots();
	CloseInterfaces();

	if (inventory_ != nullptr) {
		delete inventory_;
		inventory_ = nullptr;
	}
}

void Backpack::Pack( void )
{
	// Pack individual buttons.
	equipButton_->Pack();
	craftButton_->Pack();
	sortButton_->Pack();

	// Pack button layout.
	buttonLayout_->Pack();

	// Pack top layout (everything else already packed).
	backpackLayout_->Pack();
}

void Backpack::LoadInterfaces( void )
{
	Steam::LoadInterfaces();
}

void Backpack::CloseInterfaces( void )
{
	Steam::CloseInterfaces();
}

void Backpack::Precache( DirectX *directX )
{
	// Get button textures.
	Texture *craftTexture = directX->GetTexture( "manager/gear" );
	Texture *equipTexture = directX->GetTexture( "manager/equip" );
	Texture *sortTexture = directX->GetTexture( "manager/sort" );
	craftButton_->SetIcon( craftTexture );
	equipButton_->SetIcon( equipTexture );
	sortButton_->SetIcon( sortTexture );
	Pack();
}

void Backpack::Release( void )
{
	// Nothing that's not automatically released.
}

void Backpack::HandleCallback( int id, void *callback )
{
}

void Backpack::HandleMessage( int id, void *message, uint32 size )
{
	switch (id) {
	case SOMsgCacheSubscribed_t::k_iMessage:
		{
			if ( IsLoaded() ) {
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
						if ( GetSteamId() == subscribedMsg.owner() ) {
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

								if (econItem.has_custom_name()) {
									item->SetCustomName( econItem.custom_name() );
								}

								inventory_->InsertItem( item );
							}
						}

						break;
					}
				case 7:
					{
						if ( subscribedMsg.owner() == GetSteamId() ) {
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

			SetLoaded( true );
			inventory_->UpdateExcluded();
			notifications_->AddNotification( "Backpack successfully loaded from Steam.", nullptr );
			break;
		}

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
}

void Backpack::CreateInventory( int width, int height, int pages, int excludedSize )
{
	inventory_ = new Inventory( width, height, pages, excludedSize );
	FormatInventory();
}

void Backpack::LoadInventory( const string &jsonInventory )
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
}

void Backpack::FormatInventory( void )
{
	// Add slots to layout.
	pages_ = new HorizontalLayout();
	pages_->SetParent( this );
	pages_->SetSpacing( PAGE_SPACING );

	int pages = inventory_->GetPageCount();
	int pageWidth = inventory_->GetWidth();
	int pageCapacity = inventory_->GetPageSize();
	for (int i = 0; i < pages; i++) {
		// Add a column per page.
		GridLayout *page = new GridLayout( pageWidth );
		page->SetSpacing( SLOT_SPACING );
		page->SetParent( this );

		// Split the page into two.
		for (int j = 0; j < pageCapacity; j++) {
			int index = j + (i * pageCapacity);
			Slot* slot = inventory_->GetInventorySlot( index );
			page->Add( slot );
		}
		
		page->Pack();
		pages_->Add( page );
	}

	pages_->Pack();

	// Position and add excluded.
	excluded_ = new HorizontalLayout();
	excluded_->SetSpacing( SLOT_SPACING );

	int length = inventory_->GetExcludedSize();
	for (int i = 0; i < length; i++) {
		Slot* slot = inventory_->GetExcludedSlot( i );
		excluded_->Add( slot );
	}

	excluded_->Pack();

	// Set primary camera target.
	UpdateTarget();
	UpdateChildren();
}

void Backpack::SetNotificationQueue( NotificationQueue *notifications )
{
	notifications_ = notifications;
}

bool Backpack::IsLoaded( void ) const
{
	return isLoaded_;
}

void Backpack::SetLoaded( bool isLoaded )
{
	isLoaded_ = isLoaded;
}

void Backpack::MoveItem( Slot *source, Slot *destination ) {
	// Avoid redundancy.
	if (source == destination) {
		return;
	}

	Item* sourceItem = source->GetItem();
	Item* destItem = destination->GetItem();

	// Update items.
	inventory_->MoveItem( source, destination );
	UpdateItem( sourceItem ); // Definitely not null.
	if (destItem != nullptr) {
		UpdateItem( destItem );
	}
}

bool Backpack::OnMouseMoved( Mouse *mouse )
{
	// Hover over buttons.
	equipButton_->OnMouseMoved( mouse );
	craftButton_->OnMouseMoved( mouse );
	sortButton_->OnMouseMoved( mouse );

	// Move dragged.
	if (selected_.size() == 1 && dragged_ != nullptr) {
		Slot* slot = selected_[0];

		// Update slot position since dragging.
		slot->UpdateChildren();
		SetHovering( nullptr );

		// Change page if at edges.
		int time = GetTickCount();
		if (time > pageDelay_) {
			float mouseX = mouse->GetX();
			float backpackX = GetGlobalX();
			if (mouseX >= backpackX + GetWidth()) {
				NextPage();
				pageDelay_ = time + PAGE_CHANGE_DELAY;
			}
			else if (mouseX <= backpackX) {
				PrevPage();
				pageDelay_ = time + PAGE_CHANGE_DELAY;
			}
		}

		return true;
	}

	// Go through visible pages.
	deque<Component*> *pages = pages_->GetChildren();
	for each (Container *page in *pages) {
		if (IsVisible( page )) {
			// Go through visible page columns.
			deque<Component*> *slots = page->GetChildren();
			for each (Slot *slot in *slots) {
				if (mouse->IsTouching( slot )) {
					SetHovering( slot->HasItem() ? slot : nullptr );
					return true;
				}
			}
		}
	}

	// Check excluded.
	int length = inventory_->GetExcludedSize();
	for (int i = 0; i < length; i++) {
		Slot *slot = inventory_->GetExcludedSlot( i );
		if (mouse->IsTouching( slot )) {
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
	deque<Component*> *pages = pages_->GetChildren();
	for each (Container *page in *pages) {
		if (IsVisible( page )) {
			// Go through visible page columns.
			deque<Component*> *slots = page->GetChildren();
			for each (Slot *slot in *slots) {
				if (mouse->IsTouching( slot )) {
					if (dragged_ == nullptr) {
						OnSlotGrabbed( mouse, slot );
					}

					return true;
				}
			}
		}
	}

	// Check excluded.
	int length = inventory_->GetExcludedSize();
	for (int i = 0; i < length; i++) {
		Slot *slot = inventory_->GetExcludedSlot( i );
		if (mouse->IsTouching( slot )) {
			if (dragged_ == nullptr) {
				OnSlotGrabbed( mouse, slot );
			}
			return true;
		}
	}

	// Check buttons.
	if (equipButton_->OnLeftClicked( mouse ) ||
		craftButton_->OnLeftClicked( mouse ) ||
		sortButton_->OnLeftClicked( mouse )) {
			return true;
	}

	// Nothing hit, deselect all.
	if (selectMode_ != SELECT_MODE_MULTIPLE) {
		DeselectAll();
	}

	return false;
}

bool Backpack::OnLeftReleased( Mouse *mouse )
{
	// Check buttons.
	if (craftButton_->OnLeftReleased( mouse )) {
		CraftSelected();
		return true;
	}

	if (dragged_ != nullptr) {
		// Move item back to slot.
		Slot* selectedSlot = selected_[0];
		dragged_->SetItem( selectedSlot->GetItem() );

		// Remove dummy slot.
		Remove( selectedSlot );
		delete selectedSlot;
		selected_.clear();

		// Go through visible pages.
		deque<Component*> *pages = pages_->GetChildren();
		for each (Component *pageComponent in *pages) {
			Container *page = static_cast<Container*>(pageComponent);
			if (IsVisible( page )) {
				// Go through visible page columns.
				deque<Component*> *slots = page->GetChildren();
				for each (Component *slotComponent in *slots) {
					Slot *slot = static_cast<Slot*>(slotComponent);
					// Go through slots in column.
					if (mouse->IsTouching( slot )) {
						OnSlotReleased( slot );
						dragged_ = nullptr;
						return true;
					}
				}
			}
		}

		// Dragged was not moved.
		SelectSlot( dragged_, SELECT_TYPE_NORMAL );
		dragged_ = nullptr;
		return true;
	}

	return false;
}

void Backpack::OnSlotGrabbed( Mouse *mouse, Slot *slot )
{
	// Clear selected.
	if (selectMode_ == SELECT_MODE_SINGLE) {
		DeselectAll();
		if (slot->GetItem() != nullptr) {
			dragged_ = slot;

			// Create a dummy slot to drag the item.
			Slot* dragging = new Slot( 
				slot->GetIndex(), 
				slot->GetItem() );
			dragging->SetGlobalPosition( slot->GetGlobalX(), slot->GetGlobalY() );
			dragging->SetGroup( slot->GetGroup() );
			dragging->SetParent( this );
			Add( dragging );
		
			// Remove item from slot.
			slot->SetItem( nullptr );

			// Start dragging.
			dragging->OnDrag( mouse );
			SelectSlot( dragging, SELECT_TYPE_DRAG );
		}
	}
	else {
		if (slot->GetItem() != nullptr) {
			switch (slot->GetSelectType()) {
			case SELECT_TYPE_NONE:
				SelectSlot( slot, SELECT_TYPE_NORMAL );
				break;
			default:
				DeselectSlot( slot );
				break;
			}
		}
	}
}

void Backpack::OnSlotReleased( Slot *slot )
{
	// Skip if returning or excluded.
	if (!slot->HasItem() || dragged_->GetGroup() == GROUP_INVENTORY) {
		// Move to slot if able.
		MoveItem( dragged_, slot );
		SelectSlot( slot, SELECT_TYPE_NORMAL );
	}
	else {
		// Set to regular select.
		SelectSlot( dragged_, SELECT_TYPE_NORMAL );
	}
}

void Backpack::RemoveSlots( void ) {
	if (pages_ != nullptr) {
		deque<Component*> *pages = pages_->GetChildren();
		for each (Container *page in *pages) {
			page->RemoveAll();
		}
	}

	if (excluded_ != nullptr) {
		excluded_->RemoveAll();
	}
}

void Backpack::EquipItem( Item *item, uint32 classFlags ) {
	if (item->IsEquipped( classFlags )) {
		item->SetEquip( classFlags, false );
	}
	else {
		UnequipItems( classFlags, item->GetEquipSlot() );
		item->SetEquip( classFlags, true );
	}

	UpdateItem( item );
}

void Backpack::UnequipItems( uint32 equipClass, EItemSlot slot ) {
	const itemMap* inventoryItems = inventory_->GetInventoryItems();
	itemMap::const_iterator i;
	for (i = inventoryItems->begin(); i != inventoryItems->end(); i++) {
		Item *item = i->second;
		if (item->IsEquipped( equipClass )) {
			if (item->GetEquipSlot() == slot) {
				item->SetEquip( equipClass , false );
				UpdateItem( item );
				break;
			}
		}
	}

	const itemMap *excludedItems = inventory_->GetExcludedItems();
	for (i = excludedItems->begin(); i != excludedItems->end(); i++) {
		Item *item = i->second;
		if (item->IsEquipped( equipClass )) {
			if (item->GetEquipSlot() == slot) {
				item->SetEquip( equipClass , false );
				UpdateItem( item );
				break;
			}
		}
	}
}

void Backpack::SelectSlot( Slot* slot, ESelectType selectType )
{
	slot->SetSelectType( selectType );
	selected_.push_back( slot );
	UpdateButtons();
}

void Backpack::DeselectSlot( Slot* slot )
{
	// Deselect and remove from selected.
	slot->SetSelectType( SELECT_TYPE_NONE );
	slotVector::iterator i = find( selected_.begin(), selected_.end(), slot );
	selected_.erase( i );
	UpdateButtons();
}

void Backpack::DeselectAll( void )
{
	// Deselect all.
	while (!selected_.empty()) {
		Slot *slot = selected_.back();
		DeselectSlot( slot );
	}
}

void Backpack::CraftSelected( void )
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
}

void Backpack::UpdateButtons( void )
{
	// Adjust button states.
	switch (selected_.size()) {
	case 0:
		equipButton_->SetEnabled( false );
		craftButton_->SetEnabled( false );
		break;
	case 1:
		{
			// Check whether item is equippable.
			Item *selected = selected_[0]->GetItem();
			equipButton_->SetEnabled( selected->GetEquipClassCount() != 0 );
			craftButton_->SetEnabled( true );
		}

		break;
	default:
		equipButton_->SetEnabled( false );
		craftButton_->SetEnabled( true );
		break;
	}
}

void Backpack::UpdateItem( Item* item )
{
	// Generate message with new flags.
	// TODO: See if we can use protobufs here before old messages are deprecated.
	GCSetItemPosition_t message;
	memset( &message, 0xFF, sizeof( message ) );
	message.itemID = item->GetUniqueId();
	message.position = item->GetFlags();

	// Send it.
	SendMessage( GCSetItemPosition_t::k_iMessage, &message, sizeof( message ) );
}

slotVector* Backpack::GetSelected( void )
{
	return &selected_;
}

void Backpack::SetSelectMode( ESelectMode selectMode)
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

void Backpack::UpdateTarget( void )
{
	deque<Component*>* pageColumns = pages_->GetChildren();
	Component *cameraTarget = pageColumns->at( page_ - 1 );
	cameraDest_ = -cameraTarget->GetLocalX();
}

void Backpack::NextPage( void )
{
	if (page_ < inventory_->GetPageCount()) {
		page_++;
	}

	UpdateTarget();
}

void Backpack::PrevPage( void )
{
	if (page_ > 1) {
		page_--;
	}

	UpdateTarget();
}

void Backpack::HandleCamera( void )
{
	if (pages_->GetLocalX() != cameraDest_) {
		MoveCamera();
	}
}

void Backpack::MoveCamera( void )
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
}
