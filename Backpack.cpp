#include "Backpack.h"

// Drawing constants.
const float	SLOT_SPACING 		= 5.0f;
const float	PADDING				= 25.0f;
const float EXCLUDED_Y			= 415.0f;

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
	slotVector::const_iterator i;
	const slotVector* inventory = getInventory();
	for (i = inventory->begin(); i != inventory->end(); i++) {
		Slot* slot = *i;
		add( slot );
		slot->setMouseListener( this );
	}

	// Position and add excluded.
	const slotVector* excluded = getExcluded();
	for (int k = 0; k < excluded->size(); k++) {
		Slot* slot = excluded->at( k );
		slot->setPosition( PADDING + getX() + k * (Slot::texture->getWidth() + SLOT_SPACING), EXCLUDED_Y );

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

	slotVector::const_iterator i;
	const slotVector* inventory = getInventory();
	for (i = inventory->begin(); i != inventory->end(); i++) {
		Slot* slot = *i;

		// Only draw if visible.
		int slotWidth = slot->getWidth();
		if ((slot->getX() > -slotWidth) && (slot->getX() < getWidth())) {
			slot->draw( directX );
		}
	}

	// Draw excluded.
	const slotVector* excluded = getExcluded();
	for (i = excluded->begin(); i != excluded->end(); i++) {
		Slot* slot = *i;
		slot->draw( directX );
	}

	// Draw selected last.
	for (i = selected_.begin(); i != selected_.end(); i++) {
		Slot* slot = *i;
		slot->draw( directX );
	}
}

void Backpack::updatePosition()
{
	moveCamera();

	// Position all slots.
	const slotVector* inventory = getInventory();
	slotVector::const_iterator i;
	for (i = inventory->begin(); i != inventory->end(); i++) {
		Slot* slot = *i;

		int index = slot->getIndex();
		int x = index % invWidth_;
		int y = index / invWidth_;

		float pageOffset = 0;
		if (y >= invHeight_) {
			pageOffset = getWidth() * (y / invHeight_);
			y %= invHeight_;
		}
		
		// Set position.
		float slotX = PADDING + pageOffset + getX() + x * (Slot::texture->getWidth() + SLOT_SPACING) - cameraX_;
		float slotY = PADDING + getY() + y*(Slot::texture->getHeight() + SLOT_SPACING);
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
		const slotVector* inventory = getInventory();
		slotVector::const_iterator i;
		for (i = inventory->begin(); i != inventory->end(); i++) {
			Slot *slot = *i;
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
	slotVector::const_iterator i;

	// Remove inventory.
	const slotVector* slots = getInventory();
	for (i = slots->begin(); i != slots->end(); i++) {
		remove( *i );
	}

	// Remove excluded.
	slots = getExcluded();
	for (i = slots->begin(); i != slots->end(); i++) {
		remove( *i );
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