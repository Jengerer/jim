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
	page_ = excludedPage_ = 1;

	// Move to start.
	setPosition( x, y );
	setSize( parent->getWidth(), parent->getHeight() );

	// Add all slots to the container.
	const slotArray inventory = getInventory();
	int i, length = getCapacity();
	for (i = 0; i < length; i++) {
		Slot* slot = inventory[i];
		add( slot );
		slot->setMouseListener( this );
	}

	// Position and add excluded.
	const slotArray excluded = getExcluded();
	length = EXCLUDED_WIDTH;
	for (i = 0; i < length; i++) {
		Slot* slot = excluded[i];
		slot->setPosition( BACKPACK_PADDING_X + getX() + i * (slot->getWidth() + SLOT_SPACING), EXCLUDED_Y );

		add( slot );
		slot->setMouseListener( this );
	}
}

Backpack::~Backpack()
{
	// Backpack has been destroyed.
	removeSlots();
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
		float slotX = BACKPACK_PADDING_X + pageOffset + getX() + x * (slotWidth + SLOT_SPACING) - cameraX_;
		float slotY = BACKPACK_PADDING_Y + getY() + y * (slotHeight + SLOT_SPACING);
		slot->setPosition( slotX, slotY );
	}
}

void Backpack::select( Slot* slot )
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

bool Backpack::mouseEvent( Mouse *mouse, EMouseEvent eventType )
{
	if (Container::mouseEvent( mouse, eventType )) {
		// Item has been hit.
		return true;
	}
	else if (eventType == MOUSE_EVENT_CLICK) {
		// Nothing hit, deselect everything.
		for (int i = selected_.size() - 1; i >= 0; i--) {
			selected_[0]->setSelectType( SELECT_TYPE_NONE );
			selected_.pop_back();
		}
	}

	return false;
}

void Backpack::mouseClicked( Mouse *mouse, Component *component )
{
	// Mouse clicked.
	Slot* slot = (Slot*)component;

	// Clear selected.
	// TODO: Only clear if not CTRL.
	for (int i = selected_.size() - 1; i >= 0; i--) {
		selected_[0]->setSelectType( SELECT_TYPE_NONE );
		selected_.pop_back();
	}

	if (slot->getItem()) {
		// Create a new slot.
		Slot* dragging = new Slot( *slot );
		dragging->setIndex( slot->getIndex() );
		dragging->setGroup( slot->getGroup() );
		dragging->setItem( slot->getItem() );

		// Drag and add.
		dragging->setSelectType( SELECT_TYPE_DRAG );
		add( dragging );
		
		// Remove item from slot.
		slot->setItem( 0 );

		// Start dragging.
		dragging->onDrag( mouse, this );
		selected_.push_back( dragging );
	}
}

void Backpack::mouseReleased( Mouse *mouse, Component *component )
{
	if ((selected_.size() == 1) && (selected_[0] == component)) {
		// Which slot to move to.
		Slot* selectedSlot = selected_[0];
		Slot* newSlot = 0;

		// Check for slot collision.
		const slotArray inventory = getInventory();
		int i, length = getCapacity();
		for (i = 0; i < length; i++) {
			Slot *slot = inventory[i];
			if (mouse->isTouching( slot )) {
				if (slot->getItem() == 0) {
					newSlot = slot;
					move( selectedSlot, slot );
				}
				else if (selectedSlot->getGroup() == GROUP_INVENTORY) {
					// Move item to new position.
					newSlot = slot;
					Item* oldItem = slot->getItem();
					Item* newItem = selectedSlot->getItem();

					// Now set new slot and re-insert old.
					selectedSlot->setItem( oldItem );
					newSlot->setItem( newItem );
					insert( oldItem );
				}
			}
		}

		// If not touching slot, move back.
		if (!newSlot) {
			newSlot = insert( selectedSlot->getItem() );
		}

		// Deselect item and remove dragged.
		Slot* dragging = selected_[0];
		dragging->onRelease();
		remove( dragging );
		delete dragging;
		selected_.clear();

		// Set to regular select.
		selected_.push_back( newSlot );
		newSlot->setSelectType( SELECT_TYPE_NORMAL );

		// Move item back.
		newSlot->updatePosition();
	}
}

void Backpack::mouseMoved( Mouse *mouse, Component *component )
{
	// Mouse moved.
	if (selected_.size() == 1) {
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
	cameraSpeed_ += (cameraDest_ - cameraX_)*SPRING_STRENGTH;
	cameraSpeed_ *= SPRING_DAMPING;

	// Now propel.
	cameraX_ += cameraSpeed_;
}