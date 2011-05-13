#include "Inventory.h"

Inventory::Inventory( 
	int width, int height, 
	int pages, int excludedWidth )
{
	width_ = width;
	height_ = height;
	pages_ = pages;
	excludedWidth_ = excludedWidth;

	SetExcludedPage( 0 );
	CreateSlots();
}

Inventory::~Inventory()
{
	ClearItems();
	RemoveSlots();
}

int Inventory::GetWidth() const
{
	return width_;
}

int Inventory::GetHeight() const
{
	return height_;
}

int Inventory::GetPageCount() const
{
	return pages_;
}

Slot* Inventory::GetInventorySlot( int index ) const
{
	return &inventorySlots_[ index ];
}

Slot* Inventory::GetExcludedSlot( int index ) const
{
	return &excludedSlots_[ index ];
}

int Inventory::GetInventorySize( void ) const
{
	return GetPageSize() * GetPageCount();
}

int Inventory::GetPageSize( void ) const
{
	return GetWidth() * GetHeight();
}

int Inventory::GetExcludedSize() const
{
	return excludedWidth_;
}

Item* Inventory::GetItemByUniqueId( uint64 id )
{
	// Check inventory.
	itemMap::iterator i = inventoryItems_.find( id );
	if (i != inventoryItems_.end()) {
		return i->second;
	}

	// Check excluded.
	i = excludedItems_.find( id );
	if (i != excludedItems_.end()) {
		return i->second;
	}

	return nullptr;
}

const itemMap*	Inventory::GetInventoryItems( void ) const
{
	return &inventoryItems_;
}

const itemMap*	Inventory::GetExcludedItems( void ) const
{
	return &excludedItems_;
}

void Inventory::CreateSlots( void )
{
	// Generate slot arrays.
	inventorySlots_ = new Slot[ GetInventorySize() ];
	excludedSlots_ = new Slot[ GetExcludedSize() ];

	// Create slots.
	int length = GetInventorySize();
	for (int i = 0; i < length; i++) {
		Slot *slot = &inventorySlots_[ i ];
		slot->SetGroup( GROUP_INVENTORY );
		slot->SetIndex( i );
	}

	// Create excluded slots.
	length = GetExcludedSize();
	for (int i = 0; i < length; i++) {
		Slot *slot = &excludedSlots_[ i ];
		slot->SetGroup( GROUP_EXCLUDED );
	}
}

void Inventory::AddSlots( unsigned int numSlots )
{
	int extraPages = numSlots / GetPageSize();
	SetPageCount( GetPageCount() + extraPages );
}

void Inventory::RemoveSlots( void )
{
	// Remove inventory.
	if (inventorySlots_ != nullptr) {
		delete[] inventorySlots_;
		inventorySlots_ = nullptr;
	}

	// Remove excluded.
	if (excludedSlots_ != nullptr) {
		delete[] excludedSlots_;
		excludedSlots_ = nullptr;
	}
}

void Inventory::EmptySlots( void )
{
	int i, length;
	if (inventorySlots_ != nullptr) {
		length = GetInventorySize();
		for (i = 0; i < length; i++) {
			Slot* slot = &inventorySlots_[i];
			slot->SetItem( nullptr );
		}
	}

	if (excludedSlots_ != nullptr) {
		length = GetExcludedSize();
		for (i = 0; i < length; i++) {
			Slot* slot = &excludedSlots_[i];
			slot->SetItem( nullptr );
		}
	}
}

int Inventory::GetExcludedPage() const
{
	return excludedPage_;
}

void Inventory::SetExcludedPage( int page )
{
	excludedPage_ = page;
}

void Inventory::UpdateExcluded( void ) {
	itemMap::iterator k = excludedItems_.begin();
	for (int i = 0; i < GetExcludedSize(); i++) {
		Slot *slot = &excludedSlots_[i];
		if (k != excludedItems_.end()) {
			slot->SetItem( k->second );
			k++;
		}
		else {
			slot->SetItem( nullptr );
		}
	}
}

// Inserts an item, adds it to list, and returns its slot, if any.
Slot* Inventory::AddItem( Item* item )
{
	Slot* slot = InsertItem( item );
	if (slot == nullptr || slot->GetGroup() == GROUP_EXCLUDED) {
		ToExcluded( item );
		UpdateExcluded();
	}
	else {
		ToInventory( item );
	}

	return slot;
}

// Inserts an item and returns its slot, if any.
Slot* Inventory::InsertItem( Item* item )
{
	// Add item to correct slot.
	uint32 flags = item->GetFlags();
	uint16 position = item->GetIndex();
	uint32 leftBytes = flags & 0xf0000000;
	if ((leftBytes == 0x80000000) && (flags & 0xfff) && CanMove( position )) {
		Slot* destination = GetInventorySlot( position );
		destination->SetItem( item );
		return destination;
	}

	return nullptr;
}

void Inventory::RemoveItem( uint64 uniqueId )
{
	// Search for item in inventory.
	itemMap::iterator i = inventoryItems_.find( uniqueId );
	if (i != inventoryItems_.end()) {
		Item *item = i->second;
		Slot *slot = GetInventorySlot( item->GetIndex() );
		slot->SetItem( nullptr );
		inventoryItems_.erase( i );
		delete item;
		return;
	}
	
	// Search for item in excluded.
	i = excludedItems_.find( uniqueId );
	if (i != excludedItems_.end()) {
		Item *item = i->second;
		delete item;
		excludedItems_.erase( i );
		UpdateExcluded();
		return;
	}
}

void Inventory::ClearItems( void )
{
	itemMap::iterator i;
	for (i = inventoryItems_.begin(); i != inventoryItems_.end(); i++) {
		delete i->second;
	}

	for (i = excludedItems_.begin(); i != excludedItems_.end(); i++) {
		delete i->second;
	}

	// All items deleted, empty maps.
	inventoryItems_.clear();
	excludedItems_.clear();
}

void Inventory::ToInventory( Item *item )
{
	// Remove from excluded.
	excludedItems_.erase( item->GetUniqueId() );
	itemPair newPair( item->GetUniqueId(), item );
	inventoryItems_.insert( newPair );
}

void Inventory::ToExcluded( Item *item )
{
	// Remove from inventory.
	inventoryItems_.erase( item->GetUniqueId() );
	itemPair newPair( item->GetUniqueId(), item );
	excludedItems_.insert( newPair );
}

void Inventory::MoveItem( Slot *source, Slot *destination )
{
	// Check if needs transfer.
	Item* target = source->GetItem();
	if (source->GetGroup() == GROUP_EXCLUDED) {
		// Disallow movement to occupied inventory.
		Item* other = destination->GetItem();
		if (destination->GetGroup() == GROUP_INVENTORY && other == nullptr) {
			// Remove item from excluded.
			ToInventory( target );
			UpdateExcluded();
		}
	}
	else {
		source->SetItem( destination->GetItem() );
	}

	// Move items.
	destination->SetItem( target );
}

bool Inventory::IsValidIndex( uint16 index ) const
{
	return (index >= 0 && index < GetInventorySize());
}

bool Inventory::CanMove( uint16 index ) const
{
	return IsValidIndex( index ) && (GetInventorySlot( index )->GetItem() == nullptr);
}

void Inventory::SetWidth( int width )
{
	width_ = width;
}

void Inventory::SetHeight( int height )
{
	height_ = height;
}

void Inventory::SetPageCount( int pageCount )
{
	pages_ = pageCount;
}