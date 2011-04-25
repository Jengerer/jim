#include "Backpack.h"

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
	add( pages_ );
	add( excluded_ );

	// Slot state information.
	dragged_ = nullptr;
	hovered_ = nullptr;

	// Starting attributes.
	isLoaded_ = false;
	cameraSpeed_ = 0;
	page_ = 1;

	// Move to start.
	setPosition( x, y );
	setSize( parent->getWidth(), parent->getHeight() );

	// Create item display.
	itemDisplay_ = new ItemDisplay();
	add( itemDisplay_ );
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
	switch (id)
	{
	case SOMsgCacheSubscribed_t::k_iMessage:
		{
			// Start loading items.
			// CMsgSOCacheSubscribed cacheSubscribedMsg;
			SerializedBuffer serializedBuffer( message );
			SOMsgCacheSubscribed_t *list = serializedBuffer.get<SOMsgCacheSubscribed_t>();
			SOMsgCacheSubscribed_Items_t *items = serializedBuffer.get<SOMsgCacheSubscribed_Items_t>();

			// Don't load if we've already loaded.
			if (!isLoaded()) {
				for (int i = 0; i < items->itemcount; i++) {
					SOMsgCacheSubscribed_Item_t *item = serializedBuffer.get<SOMsgCacheSubscribed_Item_t>();

					// Get custom item name.
					char* customName = 0;
					if (item->namelength > 0) {
						customName = (char*)serializedBuffer.here();
						serializedBuffer.push( item->namelength );
					}

					uint8* flags = serializedBuffer.get<uint8>();
					uint8* origin		= serializedBuffer.get<uint8>();
					uint16* descLength	= serializedBuffer.get<uint16>();

					// Get custom description.
					char* customDesc	= 0;
					if (*descLength > 0) {
						customDesc = (char*)serializedBuffer.here();
						serializedBuffer.push( *descLength );
					}

					serializedBuffer.push( sizeof( uint8 ) ); // Skip unknown.
					uint16* attribCount	= serializedBuffer.get<uint16>();

					// Skip past attributes.
					serializedBuffer.push( sizeof(SOMsgCacheSubscribed_Item_Attrib_t) * (*attribCount) );

					// Push past 64-bit container.
					serializedBuffer.push( sizeof(uint64) );

					//Create a new item from the information.
					Item *newItem = new Item(
						item->itemid,
						item->itemdefindex,
						item->itemlevel,
						(EItemQuality)item->itemquality,
						item->quantity,
						item->position );

					// Add the item.
					inventory_->addItem( newItem );
				}
			}

			setLoaded( true );
			inventory_->updateExcluded();
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
			inventory_->addItem( newItem );
			inventory_->updateExcluded();
			break;
		}
		
	case SOMsgDeleted_t::k_iMessage:
		{
			SOMsgDeleted_t *deleteMsg = (SOMsgDeleted_t*)message;
			Item *target = inventory_->getItem( deleteMsg->itemid );
			if (target != nullptr) {
				inventory_->removeItem( target );
				inventory_->updateExcluded();

				// Check if item is display target.
				if (itemDisplay_->getItem() == target) {
					itemDisplay_->setItem( nullptr );
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
		inventory_->addItem( newItem );
	}
}

void Backpack::formatInventory()
{
	// Add slots to layout.
	pages_->setParent( this );
	pages_->setPosition( BACKPACK_PADDING, BACKPACK_PADDING_TOP );
	pages_->setSpacing( PAGE_SPACING );

	int pages = inventory_->getPages();
	int pageWidth = inventory_->getWidth();
	int pageHeight = inventory_->getHeight();
	for (int i = 0; i < pages; i++) {
		// Add a column per page.
		HorizontalLayout *pageColumns = new HorizontalLayout();
		pageColumns->setParent( this );
		pageColumns->setSpacing( SLOT_SPACING );

		// Split the page into two.
		for (int j = 0; j < 2; j++) {
			// Add a row of slots per column.
			for (int x = 0; x < (pageWidth >> 1); x++) {
				VerticalLayout *rows = new VerticalLayout();
				rows->setSpacing( SLOT_SPACING );

				// Add a slot per row.
				for (int y = 0; y < pageHeight; y++) {
					// Simplified formula for index.
					int index = x + j * (pageWidth >> 1) + pageWidth * (i * pageHeight + y);
					Slot* slot = inventory_->getInventorySlot( index );
					rows->add( slot );
				}

				rows->pack();
				pageColumns->add( rows );
			}
		}

		pageColumns->pack();
		pages_->add( pageColumns );
	}

	pages_->pack();

	// Position and add excluded.
	excluded_->setPosition( getX() + BACKPACK_PADDING, getHeight() - SLOT_HEIGHT - BACKPACK_PADDING );
	excluded_->setSpacing( SLOT_SPACING );

	int length = inventory_->excludedSize();
	for (int i = 0; i < length; i++) {
		Slot* slot = inventory_->getExcludedSlot( i );
		excluded_->add( slot );
	}

	excluded_->pack();

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
	Item* sourceItem = source->getItem();
	Item* destItem = destination->getItem();

	// Update items.
	inventory_->moveItem( source, destination );
	updateItem( sourceItem ); // Definitely not null.
	if (destItem != nullptr) {
		updateItem( destItem );
	}
}

void Backpack::updatePosition()
{
	// Position all slots.
	if (pages_ != nullptr) {
		pages_->setX( getX() - cameraX_ );
		pages_->updatePosition();
	}
}

bool Backpack::leftClicked( Mouse *mouse )
{
	// Go through visible pages.
	deque<Component*> *pageComponents = pages_->getChildren();
	for each (Container *page in *pageComponents) {
		if (withinBounds( page )) {
			// Go through visible page columns.
			deque<Component*> *columns = page->getChildren();
			for each (Container *column in *columns) {
				if (withinBounds( column )) {
					// Go through slots in column.
					deque<Component*> *slots = column->getChildren();
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
	int length = inventory_->excludedSize();
	for (int i = 0; i < length; i++) {
		Slot *slot = inventory_->getExcludedSlot( i );
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

bool Backpack::leftReleased( Mouse *mouse )
{
	if (dragged_ != nullptr) {
		// Move item back to slot.
		Slot* selectedSlot = selected_[0];
		dragged_->setItem( selectedSlot->getItem() );

		// Remove dummy slot.
		selectedSlot->onRelease();
		remove( selectedSlot );
		delete selectedSlot;
		selected_.clear();

		// Go through visible pages.
		deque<Component*> *pageComponents = pages_->getChildren();
		for each (Container *page in *pageComponents) {
			if (withinBounds( page )) {
				// Go through visible page columns.
				deque<Component*> *columns = page->getChildren();
				for each (Container *column in *columns) {
					if (withinBounds( column )) {
						// Go through slots in column.
						deque<Component*> *slots = column->getChildren();
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

bool Backpack::mouseMoved( Mouse *mouse )
{
	// Reset item display.
	itemDisplay_->setActive( false );
	hovered_ = nullptr;

	// Mouse moved.
	if (selected_.size() == 1 && dragged_ != nullptr) {
		Slot* slot = selected_[0];
		slot->updatePosition();

		// Change page if at edges.
		int time = GetTickCount();
		if (time > pageDelay_) {
			if (slot->getX() == (getWidth() - slot->getWidth())) {
				nextPage();
				pageDelay_ = time + PAGE_CHANGE_DELAY;
			}
			else if (slot->getX() == 0) {
				prevPage();
				pageDelay_ = time + PAGE_CHANGE_DELAY;
			}
		}

		return true;
	}

	// Go through visible pages.
	deque<Component*> *pageComponents = pages_->getChildren();
	for each (Container *page in *pageComponents) {
		if (withinBounds( page )) {
			// Go through visible page columns.
			deque<Component*> *columns = page->getChildren();
			for each (Container *column in *columns) {
				if (withinBounds( column )) {
					// Go through slots in column.
					deque<Component*> *slots = column->getChildren();
					for each (Component *rowSlot in *slots) {
						Slot *slot = dynamic_cast<Slot*>(rowSlot);
						if (mouse->isTouching( slot )) {
							// Update display if not dragging and has item.
							if ((slot->getSelectType() != SELECT_TYPE_DRAG) && (slot->getItem() != nullptr)) {
								hovered_ = slot;

								itemDisplay_->setItem( slot->getItem() );
								itemDisplay_->setPosition( slot->getX() + slot->getWidth()/2 - itemDisplay_->getWidth() / 2, slot->getY() + slot->getHeight() + DISPLAY_SPACING );
								clampChild( itemDisplay_, DISPLAY_SPACING );
								itemDisplay_->setActive( true );
							}

							return true;
						}
					}
				}
			}
		}
	}

	// Check excluded.
	int length = inventory_->excludedSize();
	for (int i = 0; i < length; i++) {
		Slot *slot = inventory_->getExcludedSlot( i );
		if (mouse->isTouching( slot )) {
			// Update display if not dragging and has item.
			if ((slot->getSelectType() != SELECT_TYPE_DRAG) && (slot->getItem() != 0)) {
				hovered_ = slot;

				itemDisplay_->setItem( slot->getItem() );
				itemDisplay_->setPosition( slot->getX() + slot->getWidth()/2 - itemDisplay_->getWidth() / 2, slot->getY() + slot->getHeight() + DISPLAY_SPACING );

				// Check that we're not exceeding screen bottom.
				int bottomBound = getY() + getHeight() - slot->getHeight() - DISPLAY_SPACING;
				if (itemDisplay_->getY() > bottomBound) {
					itemDisplay_->setY( slot->getY() - slot->getHeight() - DISPLAY_SPACING );
				}

				clampChild( itemDisplay_, DISPLAY_SPACING );
				itemDisplay_->setActive( true );
			}

			return true;
		}
	}

	return false;
}

void Backpack::slotGrabbed( Mouse *mouse, Slot *slot )
{
	// Clear selected.
	if (selectMode_ == SELECT_MODE_SINGLE) {
		deselectAll();
		if (slot->getItem() != nullptr) {
			dragged_ = slot;

			// Create a dummy slot to drag the item.
			Slot* dragging = new Slot( *slot );
			dragging->setIndex( slot->getIndex() );
			dragging->setGroup( slot->getGroup() );
			dragging->setItem( slot->getItem() );
			dragging->setParent( this );
			add( dragging );
		
			// Remove item from slot.
			slot->setItem( nullptr );

			// Start dragging.
			dragging->onDrag( mouse );
			select( dragging, SELECT_TYPE_DRAG );
		}
	}
	else {
		if (slot->getItem() != 0) {
			switch (slot->getSelectType()) {
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
		if (slot->getItem() == nullptr || dragged_->getGroup() == GROUP_INVENTORY) {
			// Move to slot if able.
			moveItem( dragged_, slot );
			selected_.push_back( slot );
			slot->setSelectType( SELECT_TYPE_NORMAL );
			slot->updatePosition();
		}
	}
	else {
		// Set to regular select.
		select( dragged_, SELECT_TYPE_NORMAL );
	}
}

void Backpack::removeSlots() {
	if (pages_ != nullptr) {
		deque<Component*> *pageComponents = pages_->getChildren();
		for each (Container *page in *pageComponents) {
			deque<Component*> *columns = page->getChildren();
			for each (Container *column in *columns) {
				column->removeAll();
			}
		}
	}

	if (excluded_ != nullptr) {
		excluded_->removeAll();
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

	if (item->isEquipped( equipClass )) {
		item->setEquip( equipClass, false );
	}
	else {
		unequipItems( equipClass, item->getSlot() );
		item->setEquip( equipClass, true );
	}

	updateItem( item );
}

void Backpack::unequipItems( EClassEquip equipClass, const string& slot ) {
	itemVector* inventoryItems = inventory_->getInventoryItems();
	itemVector* excludedItems = inventory_->getExcludedItems();
	for each(Item *item in *inventoryItems) {
		if (item->isEquipped( equipClass )) {
			if (item->getSlot() == slot) {
				item->setEquip( equipClass , false );
				updateItem( item );
				break;
			}
		}
	}

	for each (Item *item in *excludedItems) {
		if (item->isEquipped( equipClass )) {
			if (item->getSlot() == slot) {
				item->setEquip( equipClass , false );
				updateItem( item );
				break;
			}
		}
	}
}

void Backpack::select( Slot* slot, ESelectType selectType )
{
	slot->setSelectType( selectType );
	selected_.push_back( slot );
}

void Backpack::deselect( Slot* slot )
{
	slot->setSelectType( SELECT_TYPE_NONE );

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
			Item *item = slot->getItem();

			// Get ID and write.
			uint64 itemId = item->getUniqueId();
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

	message.itemID = item->getUniqueId();
	message.position = item->getFlags();

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

void Backpack::updateTarget()
{
	deque<Component*>* pageColumns = pages_->getChildren();
	Component *cameraTarget = pageColumns->at( page_ - 1 );
	cameraDest_ = cameraTarget->getX() - pages_->getX() - BACKPACK_PADDING;
}

void Backpack::nextPage()
{
	if (page_ < inventory_->getPages()) {
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
	updatePosition();
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

Slot* Backpack::getHovered()
{
	return hovered_;
}