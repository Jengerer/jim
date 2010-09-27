#include "Inventory.h"

const float	SLOT_SPACING = 5.0f;

Inventory::Inventory( 
	Window* window,
	float x, float y,
	int width, int height, 
	int pages ) : Drawable( x, y )
{
	// Set dimensions.
	width_ = width;
	height_ = height;
	pages_ = pages;

	// Set window.
	window_ = window;

	// Initially unloaded.
	isLoaded_ = false;

	// Inventory has been created.
	openInterfaces();
}

Inventory::~Inventory()
{
	// Inventory has been deleted.
	closeInterfaces();
}

void Inventory::openInterfaces()
{
	// Create slots for items.
	createSlots();
}

void Inventory::closeInterfaces()
{
	// Remove all slots and items.
	clearSlots();
	clearItems();
}

void Inventory::draw( DirectX* directX )
{
	// Draw slots.
	// TODO: Don't draw hidden items.
	vector<Slot*>::iterator i;
	for (i = inventory_.begin(); i != inventory_.end(); i++)
	{
		Slot* slot = *i;
		slot->draw( directX );
	}
}

void Inventory::onMouseEvent( MouseListener* mouseListener, EMouseEvent mEvent )
{
	vector<Slot*>::iterator iter;

	switch (mEvent) {
	case MOUSE_EVENT_MOVE:
		for (iter = inventory_.begin(); iter != inventory_.end(); iter++) {
			Slot* slot = *iter;
			slot->onMouseEvent( mouseListener, mEvent );
		}
		break;

	case MOUSE_EVENT_CLICK:
		for (iter = inventory_.begin(); iter != inventory_.end(); iter++) {
			Slot* slot = *iter;
			if (slot->mouseTouching( mouseListener )) {
				if (selected_.empty()) {
					slot->setSelectType( SELECT_TYPE_DRAG );
				}
				else {
					slot->setSelectType( SELECT_TYPE_NORMAL );
				}

				selected_.push_back( slot );
			}
		}
		break;

	case MOUSE_EVENT_RELEASE:
		for (iter = selected_.begin(); iter != selected_.end(); iter++) {
			Slot* slot = *iter;
			slot->onMouseEvent( mouseListener, mEvent );
		}
		break;	
	}
}

void Inventory::handleCallbacks()
{
	//Get waiting callback.
	CallbackMsg_t callback;
	if (getCallback( &callback ))
	{
		switch (callback.m_iCallback)
		{
		case GCMessageAvailable_t::k_iCallback:
			{
				GCMessageAvailable_t *message = (GCMessageAvailable_t *)callback.m_pubParam;
				
				uint32 size;
				if (hasMessage( &size ))
				{
					unsigned int id, realSize = 0;

					// Allocate memory.
					void* buffer = malloc( size );

					// Retrieve the message.
					getMessage( &id, buffer, size, &realSize );

					switch (id)
					{
					case SOMsgCacheSubscribed_t::k_iMessage:
						// Item information has been received.
						{
							// Start loading items.
							if (!isLoaded())
							{
								SerializedBuffer serializedBuffer(buffer);
								SOMsgCacheSubscribed_t *iList = serializedBuffer.get<SOMsgCacheSubscribed_t>();
								for (int I = 0; I < iList->itemcount; I++)
								{
									SOMsgCacheSubscribed_Item_t *pItem = serializedBuffer.get<SOMsgCacheSubscribed_Item_t>();

									// Skip past the name.
									serializedBuffer.push( pItem->namelength );

									// Get attribute count, and skip past.
									uint16* attribCount = serializedBuffer.get<uint16>();
									serializedBuffer.push<SOMsgCacheSubscribed_Item_Attrib_t>( *attribCount );

									//Create a new item from the information.
									Item *newItem = new Item(
										pItem->itemid,
										pItem->itemdefindex,
										pItem->itemlevel,
										(EItemQuality)pItem->itemquality,
										pItem->itemcount,
										pItem->position );

									// Add the item.
									add( newItem );
								}

								// Loaded now.
								setLoaded();
							}

							//TODO: Update scrolling of excluded items
							break;
						}

					case GCCraftResponse_t::k_iMessage:
						{
							GCCraftResponse_t *pResponse = (GCCraftResponse_t*)buffer;

							if (pResponse->blueprint == 0xFFFF)
							{
								//TODO: Display the dialog for crafting failed.
							}

							break;
						}
					case SOMsgCreate_t::k_iMessage:
						{
							SOMsgCreate_t *msgCreate = (SOMsgCreate_t*)buffer;
							SOMsgCacheSubscribed_Item_t* craftedItem = &msgCreate->item;

							//Make sure it's a valid item.
							if (msgCreate->unknown == 5)
								break;

							Item* newItem = new Item(
								craftedItem->itemid,
								craftedItem->itemdefindex,
								craftedItem->itemlevel,
								(EItemQuality)craftedItem->itemquality,
								craftedItem->itemcount,
								craftedItem->position);

							// Add this item to excluded.
							newItem->setGroup(GROUP_EXCLUDED);
							add(newItem);

							//TODO: Update excluded scrolling.

							break;
						}
					case SOMsgDeleted_t::k_iMessage:
						{
							SOMsgDeleted_t *pDeleted = (SOMsgDeleted_t*)buffer;

							// TODO: Iterate through both vectors and remove it.

							break;
						}
					}

					// Free up memory.
					if (buffer)
						free(buffer);
				}
				
			}
		}
	}

	releaseCallback();
}

void Inventory::select( Slot* slot )
{
	// Deselect all selected.
	vector<Slot*>::iterator i;
	for (i = selected_.begin(); i != selected_.end(); i++) {
		Slot* thisSlot = *i;
		thisSlot->setSelectType( SELECT_TYPE_NONE );
	}
	selected_.clear();

	// Add this one.
	slot->setSelectType( SELECT_TYPE_NORMAL );
	selected_.push_back( slot );
}

void Inventory::clearItems()
{
	// Iterate and delete.
	vector<Item*>::iterator iter;
	while (!items_.empty()) {
		// Get this item.
		iter = items_.begin();
		Item* item = *iter;

		// Delete item.
		delete item;
		item = NULL;

		// Remove from vector.
		items_.erase(iter);
	}
}

void Inventory::createSlots()
{
	// Create slots.
	for (int i = 0; i < getCapacity(); i++) {
		Slot* slot = new Slot();

		// Calculate the real position of this slot.
		int xIndex = i % width_;
		int yIndex = i / width_;

		if (yIndex >= height_) {
			xIndex += window_->getWidth() * (yIndex / height_);
			yIndex %= height_;
		}

		float xSlot = xIndex*(Slot::texture->getWidth() + SLOT_SPACING);
		float ySlot = yIndex*(Slot::texture->getHeight() + SLOT_SPACING);

		// Set position.
		slot->x = x + xSlot;
		slot->y = y + ySlot;

		// Add to inventory.
		inventory_.push_back( slot );
	}
}

void Inventory::clearSlots()
{
	// Clear selected.
	selected_.clear();

	// Iterator for slots.
	vector<Slot*>::iterator i;

	// Delete inventory.
	for (i = inventory_.begin(); i != inventory_.end(); i++)
		delete *i;

	// Delete excluded.
	for (i = excluded_.begin(); i != excluded_.end(); i++)
		delete *i;

	// Clear vectors.
	inventory_.clear();
	excluded_.clear();
}

void Inventory::add( Item* item )
{
	// Add to list.
	items_.push_back( item );

	// Add item to correct slot.
	uint8 position = item->getPosition() - 1;
	if (isValid( position )) {
		Slot* destSlot = inventory_[position];

		// Don't overlap.
		if (!destSlot->item) {
			item->setGroup( GROUP_INVENTORY );
			destSlot->item = item;
		}
		else {
			item->setGroup( GROUP_EXCLUDED );

			// Make new slot for excluded.
			destSlot = new Slot( item );
			excluded_.push_back( destSlot );
		}
	}
}

void Inventory::remove( Item* whichItem )
{
	// TODO: Find the item in inventory.
}

void Inventory::move( Slot* slot1, Slot* slot2 )
{
	// Get item we're moving.
	Item* item1 = slot1->item;

	if (!slot2->item)
	{
		slot2->item = item1;
		slot1->item = 0;

		// updateItem( item1 );
	} else if (slot1->item->getGroup() == GROUP_INVENTORY)
	{
		Item* tempItem = item1;
		slot1->item = slot2->item;
		slot2->item = tempItem;

		// updateItem ( item1 );
		// updateItem ( item2 );
	}
}

Slot* Inventory::get( uint8 index )
{
	// Search for that slot, return NULL if invalid.
	if (!isValid( index ))
		throw Exception( "Attempted to get slot with invalid index." );

	return inventory_[index];
}

int Inventory::getWidth() const
{
	int slotsWidth = width_ * Slot::texture->getWidth();
	int spacingWidth = (width_ - 1) * SLOT_SPACING;
	return ( slotsWidth + spacingWidth );
}

int Inventory::getHeight() const
{
	int slotsHeight = height_ * Slot::texture->getHeight();
	int spacingHeight = (height_ - 1) * SLOT_SPACING;
	return ( slotsHeight + spacingHeight );
}

int Inventory::getCapacity() const
{
	return (width_ * height_ * pages_);
}

bool Inventory::isValid ( uint8 index )
{
	return (index >= 0) && (index < getCapacity());
}

void Inventory::setLoaded()
{
	isLoaded_ = true;
}

bool Inventory::isLoaded() const
{
	return isLoaded_;
}