#include "Backpack.h"

// Drawing constants.
const float	SLOT_SPACING 		= 5.0f;

// Navigation constants.
const int	PAGE_CHANGE_DELAY	= 500;

// Spring constants.
const float SPRING_STRENGTH		= 0.1f;
const float SPRING_DAMPING		= 0.7f;

Backpack::Backpack(
	float x, float y,
	int width, int height,
	int pages,
	Container* parent ) : Inventory( width, height, pages ), Container( x, y )
{
	// Backpack was created.
	isLoaded_ = false;
	cameraX_ = cameraSpeed_ = cameraDest_ = 0;
	itemDisplay_ = 0;
	page_ = excludedPage_ = 1;
	dragged_ = 0;

	// Move to start.
	setPosition( x, y );
	setSize( parent->getWidth(), parent->getHeight() );

	// Add all slots to the container.
	const slotArray inventory = getInventory();
	int i, length = getCapacity();
	for (i = 0; i < length; i++) {
		Slot* slot = inventory[i];
		add( slot );
	}

	// Position and add excluded.
	const slotArray excluded = getExcluded();
	length = EXCLUDED_WIDTH;
	for (i = 0; i < length; i++) {
		Slot* slot = excluded[i];
		slot->setPosition( BACKPACK_PADDING + getX() + i * (SLOT_WIDTH + SLOT_SPACING), getHeight() - SLOT_HEIGHT - BACKPACK_PADDING );

		add( slot );
	}

	// Create item display.
	itemDisplay_ = new ItemDisplay();
}

Backpack::~Backpack()
{
	// Backpack has been destroyed.
	removeSlots();

	// Remove item display.
	if (itemDisplay_ != 0) {
		delete itemDisplay_;
		itemDisplay_ = 0;
	}
}

void Backpack::draw( DirectX* directX )
{
	updatePosition();

	const slotArray inventory = getInventory();
	// Draw visible slots.
	int i, x, y;
	for (i = 0; i < pages_; i++) {
		for (x = 0; x < invWidth_; x++) {
			for (y = 0; y < invHeight_; y++) {
				int index = i * (invWidth_ * invHeight_) + y * invWidth_ + x;
				Slot* slot = inventory[index];

				// Only draw column if visible.
				int slotWidth = slot->getWidth();
				if ((slot->getX() <= -slotWidth) || (slot->getX() >= getWidth())) {
					break;
				}

				slot->draw( directX );
			}
		}
	}

	// Draw excluded.
	const slotArray excluded = getExcluded();
	int length = EXCLUDED_WIDTH;
	for (i = 0; i < length; i++) {
		Slot* slot = excluded[i];
		slot->draw( directX );
	}

	// Draw selected last.
	slotVector::iterator j;
	for (j = selected_.begin(); j != selected_.end(); j++) {
		Slot* slot = *j;
		slot->draw( directX );
	}

	// Draw item information.
	itemDisplay_->draw( directX );
}

void Backpack::updatePosition()
{
	moveCamera();

	// Position all slots.
	const slotArray inventory = getInventory();
	int i, length = getCapacity();
	for (i = 0; i < length; i++) {
		Slot* slot = inventory[i];

		int x = i % invWidth_;
		int y = i / invWidth_;

		float pageOffset = 0;
		if (y >= invHeight_) {
			pageOffset = getWidth() * (y / invHeight_);
			y %= invHeight_;
		}
		
		// Set position.
		int slotWidth = slot->getWidth();
		int slotHeight = slot->getHeight();
		float slotX = BACKPACK_PADDING + pageOffset + getX() + x * (slotWidth + SLOT_SPACING) - cameraX_;
		float slotY = BACKPACK_PADDING_TOP + getY() + y * (slotHeight + SLOT_SPACING);
		slot->setPosition( slotX, slotY );
	}
}

bool Backpack::mouseClicked( Mouse *mouse )
{
	// Check visible slots.
	int i, x, y;
	const slotArray inventory = getInventory();
	for (i = 0; i < pages_; i++) {
		for (x = 0; x < invWidth_; x++) {
			for (y = 0; y < invHeight_; y++) {
				int index = i * (invWidth_ * invHeight_) + y * invWidth_ + x;
				Slot* slot = inventory[index];

				// Only draw column if visible.
				int slotWidth = slot->getWidth();
				if ((slot->getX() <= -slotWidth) || (slot->getX() >= getWidth())) {
					break;
				}

				if (mouse->isTouching( slot )) {
					slotClicked( mouse, slot );
					return true;
				}
			}
		}
	}

	// Check excluded.
	const slotArray excluded = getExcluded();
	for (i = 0; i < EXCLUDED_WIDTH; i++) {
		Slot *slot = excluded[i];

		if (mouse->isTouching( slot )) {
			slotClicked( mouse, slot );
			return true;
		}
	}

	// Nothing hit, deselect all.
	if (selectMode_ != SELECT_MODE_MULTIPLE) {
		deselectAll();
	}

	return false;
}

bool Backpack::mouseReleased( Mouse *mouse )
{
	// Return item if dragging.
	if (selected_.size() == 1 && dragged_ != 0) {
		// Move item back to slot.
		Slot* selectedSlot = selected_[0];
		dragged_->setItem( selectedSlot->getItem() );

		// Remove dummy slot.
		selectedSlot->onRelease();
		delete selectedSlot;
		selected_.clear();

		// Check visible slots.
		int i, x, y;
		const slotArray inventory = getInventory();
		for (i = 0; i < pages_; i++) {
			for (x = 0; x < invWidth_; x++) {
				for (y = 0; y < invHeight_; y++) {
					int index = i * (invWidth_ * invHeight_) + y * invWidth_ + x;
					Slot* slot = inventory[index];

					// Only draw column if visible.
					int slotWidth = slot->getWidth();
					if ((slot->getX() <= -slotWidth) || (slot->getX() >= getWidth())) {
						break;
					}

					if (mouse->isTouching( slot )) {
						slotReleased( slot );
						dragged_ = 0;
						return true;
					}
				}
			}
		}

		// Dragged was not moved.
		dragged_->setSelectType( SELECT_TYPE_NORMAL );
		selected_.push_back( dragged_ );

		// Reset dragged.
		dragged_ = 0;
		return true;
	}

	return false;
}

bool Backpack::mouseMoved( Mouse *mouse )
{
	// Reset item display.
	itemDisplay_->setActive( false );

	// Mouse moved.
	if (selected_.size() == 1 && dragged_ != 0) {
		Slot* slot = selected_[0];
		slot->updatePosition();

		// Change page if at edges.
		int time = GetTickCount();
		if (time > pageDelay) {
			if (slot->getX() == (getWidth() - slot->getWidth())) {
				nextPage();
				pageDelay = time + PAGE_CHANGE_DELAY;
			}
			else if (slot->getX() == 0) {
				prevPage();
				pageDelay = time + PAGE_CHANGE_DELAY;
			}
		}

		return true;
	}

	// Check for collision with visible slots.
	int i, x, y;
	const slotArray inventory = getInventory();
	for (i = 0; i < pages_; i++) {
		for (x = 0; x < invWidth_; x++) {
			for (y = 0; y < invHeight_; y++) {
				int index = i * (invWidth_ * invHeight_) + y * invWidth_ + x;
				Slot* slot = inventory[index];

				// Only draw column if visible.
				int slotWidth = slot->getWidth();
				if ((slot->getX() <= -slotWidth) || (slot->getX() >= getWidth())) {
					break;
				}

				if (mouse->isTouching( slot ) && (slot->getItem() != 0)) {
					// Update display if not dragging.
					if (slot->getSelectType() != SELECT_TYPE_DRAG) {
						itemDisplay_->setItem( slot->getItem() );
						itemDisplay_->setPosition( slot->getX() + slot->getWidth()/2 - itemDisplay_->getWidth() / 2, slot->getY() + slot->getHeight() + 10 );

						// Bound position.
						int rightBound = getWidth() - itemDisplay_->getWidth() - DISPLAY_SPACING;
						if (itemDisplay_->getX() > rightBound) {
							itemDisplay_->setX( rightBound );
						}
						else if (itemDisplay_->getX() < DISPLAY_SPACING) {
							itemDisplay_->setX( DISPLAY_SPACING );
						}

						itemDisplay_->setActive( true );
					}

					return true;
				}
			}
		}
	}

	return false;
}

void Backpack::slotClicked( Mouse *mouse, Slot *slot )
{
	// Clear selected.
	if (selectMode_ == SELECT_MODE_SINGLE) {
		// First deselect all.
		deselectAll();

		// Now drag slot.
		if (slot->getItem() != 0) {
			// Create a new slot.
			dragged_ = slot;

			// Create a dummy slot to drag the item.
			Slot* dragging = new Slot( *slot );
			dragging->setIndex( slot->getIndex() );
			dragging->setGroup( slot->getGroup() );
			dragging->setItem( slot->getItem() );
			dragging->setParent( this );
		
			// Remove item from slot.
			slot->setItem( 0 );

			// Start dragging.
			dragging->onDrag( mouse );
			switch (slot->getSelectType()) {
			case SELECT_TYPE_NONE:
				select( dragging, SELECT_TYPE_DRAG );
				break;
			default:
				dragging->setSelectType( SELECT_TYPE_DRAG );
				break;
			}
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
	if (slot != dragged_ && slot->getGroup() == GROUP_INVENTORY) {
		if (slot->getItem() == 0 || dragged_->getGroup() == GROUP_INVENTORY) {
			// Move to slot if able.
			move( dragged_, slot );

			// Set to regular select.
			selected_.push_back( slot );
			slot->setSelectType( SELECT_TYPE_NORMAL );

			// Update position.
			slot->updatePosition();
		}
	}
	else {
		// Set to regular select.
		selected_.push_back( dragged_ );
		dragged_->setSelectType( SELECT_TYPE_NORMAL );
	}
}

void Backpack::removeSlots() {
	// Remove inventory.
	const slotArray inventory = getInventory();
	int i, length = getCapacity();
	for (i = 0; i < length; i++) {
		Slot* slot = inventory[i];
		remove( slot );
	}

	// Remove excluded.
	const slotArray excluded = getExcluded();
	length = EXCLUDED_WIDTH;
	for (i = 0; i < length; i++) {
		Slot* slot = excluded[i];
		remove( slot );
	}
}

void Backpack::move( Slot *source, Slot *destination ) {
	Item* sourceItem = source->getItem();
	Item* destItem = destination->getItem();

	// Perform standard move.
	Inventory::move( source, destination );

	// Update items.
	updateItem( sourceItem ); // Definitely not null.
	if (destItem != 0) {
		updateItem( destItem );
	}
}

Slot* Backpack::insert( Item *item )
{
	Slot* newSlot = Inventory::insert( item );
	updateItem( item );
	return newSlot;
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

void Backpack::removeItem( uint64 uniqueId )
{
	// Remove item from selection.
	slotVector::iterator i;
	for (i = selected_.begin(); i != selected_.end(); i++) {
		Slot *slot = *i;
		Item *item = slot->getItem();

		if (item && (item->getUniqueId() == uniqueId)) {
			deselect( slot );
		}
	}

	// Run default action.
	Inventory::removeItem( uniqueId );
}

void Backpack::setSelectMode( ESelectMode selectMode)
{
	selectMode_ = selectMode;
}

void Backpack::setLoaded()
{
	isLoaded_ = true;
}

bool Backpack::isLoaded() const
{
	return isLoaded_;
}

void Backpack::nextPage()
{
	if (page_ < pages_) {
		page_++;
	}
	
	cameraDest_ = (page_ - 1) * getWidth();
}

void Backpack::prevPage()
{
	if (page_ > 1) {
		page_--;
	}

	cameraDest_ = (page_ - 1) * getWidth();
}

void Backpack::moveCamera()
{
	// Add elastic speed.
	float cameraDistance = (cameraDest_ - cameraX_);
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