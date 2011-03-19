#include "Backpack.h"

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
	cameraSpeed_ = 0;
	itemDisplay_ = 0;
	page_ = excludedPage_ = 1;
	dragged_ = hovered_ = 0;

	// Layout for slot columns.
	columns_ = new HorizontalLayout();

	// Move to start.
	setPosition( x, y );
	setSize( parent->getWidth(), parent->getHeight() );

	// Add slots to layout.
	columns_->setParent( this );
	columns_->setPosition( BACKPACK_PADDING, BACKPACK_PADDING_TOP );
	columns_->setSpacing( PAGE_SPACING );

	const slotArray inventory = getInventory();
	for (int i = 0; i < pages_; i++) {
		// Add a column per page.
		HorizontalLayout *pageColumns = new HorizontalLayout();
		pageColumns->setParent( this );
		pageColumns->setSpacing( SLOT_SPACING );
		for (int x = 0; x < pageWidth_; x++) {
			// Add a row of slots per column.
			VerticalLayout *rows = new VerticalLayout();
			rows->setSpacing( SLOT_SPACING );

			for (int y = 0; y < pageHeight_; y++) {
				// Simplified formula for index.
				int index = x + pageWidth_ * (y + i * pageHeight_);
				Slot* slot = inventory[ index ];
				rows->add( slot );
			}

			rows->pack();
			pageColumns->add( rows );
		}

		pageColumns->pack();
		columns_->add( pageColumns );
	}

	columns_->pack();
	add( columns_ );

	// Position and add excluded.
	HorizontalLayout *excludedSlots_ = new HorizontalLayout();
	excludedSlots_->setPosition( getX() + BACKPACK_PADDING, getHeight() - SLOT_HEIGHT - BACKPACK_PADDING );
	excludedSlots_->setSpacing( SLOT_SPACING );
	const slotArray excluded = getExcluded();
	int i, length = EXCLUDED_WIDTH;
	for (i = 0; i < length; i++) {
		Slot* slot = excluded[i];
		excludedSlots_->add( slot );
	}
	excludedSlots_->pack();
	add( excludedSlots_ );

	// Set primary camera target.
	updateTarget();
	cameraX_ = cameraDest_;

	// Create item display.
	itemDisplay_ = new ItemDisplay();
	add( itemDisplay_ );
}

Backpack::~Backpack()
{
	// Backpack has been destroyed.
	removeSlots();
}

void Backpack::updatePosition()
{
	// Position all slots.
	columns_->setX( getX() - cameraX_ );
	columns_->updatePosition();
}

bool Backpack::leftClicked( Mouse *mouse )
{
	// Check visible slots.
	const slotArray inventory = getInventory();
	int i, length = getCapacity();
	for (i = 0; i < length; i++) {
		Slot* slot = inventory[ i ];
		if (slot->getX() < getWidth() && slot->getX() + slot->getWidth() > 0) {
			if (mouse->isTouching( slot )) {
				slotClicked( mouse, slot );
				return true;
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

bool Backpack::leftReleased( Mouse *mouse )
{
	// Return item if dragging.
	if (selected_.size() == 1 && dragged_ != 0) {
		// Move item back to slot.
		Slot* selectedSlot = selected_[0];
		dragged_->setItem( selectedSlot->getItem() );

		// Remove dummy slot.
		selectedSlot->onRelease();
		remove( selectedSlot );
		delete selectedSlot;
		selected_.clear();

		// Check visible slots.
		const slotArray inventory = getInventory();
		int i, length = getCapacity();
		for (i = 0; i < length; i++) {
			Slot* slot = inventory[ i ];
			if (mouse->isTouching( slot )) {
				slotReleased( slot );
				dragged_ = 0;
				return true;
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
	hovered_ = 0;

	// Mouse moved.
	if (selected_.size() == 1 && dragged_ != 0) {
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

	// Check for collision with visible slots.
	const slotArray inventory = getInventory();
	int i, length = getCapacity();
	for (i = 0; i < length; i++) {
		Slot* slot = inventory[ i ];
		if (slot->getX() < getWidth() && slot->getX() + slot->getWidth() > 0) {
			if (mouse->isTouching( slot ) && (slot->getItem() != 0)) {
				// Update display if not dragging.
				if (slot->getSelectType() != SELECT_TYPE_DRAG) {
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

	// Check excluded.
	const slotArray excluded = getExcluded();
	for (i = 0; i < EXCLUDED_WIDTH; i++) {
		Slot *slot = excluded[i];

		if (mouse->isTouching( slot ) && (slot->getItem() != 0)) {
			// Update display if not dragging.
			if (slot->getSelectType() != SELECT_TYPE_DRAG) {
				dragged_ = slot;

				itemDisplay_->setItem( slot->getItem() );
				itemDisplay_->setPosition( slot->getX() + slot->getWidth()/2 - itemDisplay_->getWidth() / 2, slot->getY() + slot->getHeight() + DISPLAY_SPACING );

				// Bound position.
				int rightBound = getWidth() - itemDisplay_->getWidth() - DISPLAY_SPACING;
				if (itemDisplay_->getX() > rightBound) {
					itemDisplay_->setX( rightBound );
				}
				else if (itemDisplay_->getX() < DISPLAY_SPACING) {
					itemDisplay_->setX( DISPLAY_SPACING );
				}

				// Bound vertical.
				int bottomBound = getHeight() - itemDisplay_->getHeight() - DISPLAY_SPACING;
				if (itemDisplay_->getY() > bottomBound) {
					itemDisplay_->setY( slot->getY() - itemDisplay_->getHeight() - DISPLAY_SPACING );
				}

				itemDisplay_->setActive( true );
			}

			return true;
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
			add( dragging );
		
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
		select( dragged_, SELECT_TYPE_NORMAL );
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
	itemVector* items = getItems();
	for (int i = 0; i < items->size(); i++) {
		Item *item = items->at( i );
		if (item->isEquipped( equipClass )) {
			if (item->getSlot() == slot) {
				item->setEquip( equipClass , false );
				updateItem( item );
				break;
			}
		}
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

	message.id = 1;
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

slotVector* Backpack::getSelected()
{
	return &selected_;
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

void Backpack::updateTarget()
{
	const slotArray inventory = getInventory();
	Component *cameraTarget = inventory[ (page_ - 1) * (pageWidth_ * pageHeight_) ];
	cameraDest_ = cameraTarget->getX() - columns_->getX() - BACKPACK_PADDING;
}

void Backpack::nextPage()
{
	if (page_ < pages_) {
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