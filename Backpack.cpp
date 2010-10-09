#include "Backpack.h"

const float	SLOT_SPACING = 5.0f;

Backpack::Backpack( float x, float y,
	int width, int height,
	int pages ) : Inventory( width, height, pages ), Drawable( x, y )
{
	// Backpack was created.
	isLoaded_ = false;
}

void Backpack::draw( DirectX* directX )
{
	slotVector::const_iterator i;
	const slotVector* inventory = getInventory();
	for (i = inventory->begin(); i != inventory->end(); i++) {
		Slot* slot = *i;
		slot->draw( directX );
	}
}

void Backpack::onMouseEvent( MouseListener* mouseListener, EMouseEvent mEvent )
{
	slotVector::const_iterator i;
	const slotVector* inventory = getInventory();

	switch (mEvent) {
	case MOUSE_EVENT_MOVE:
		for (i = inventory->begin(); i != inventory->end(); i++) {
			Slot* slot = *i;
			slot->onMouseEvent( mouseListener, mEvent );
		}
		break;

	case MOUSE_EVENT_CLICK:
		for (i = inventory->begin(); i != inventory->end(); i++) {
			Slot* slot = *i;
			if (slot->item && slot->mouseTouching( mouseListener )) {
				select( slot );
			}
		}
		break;

	case MOUSE_EVENT_RELEASE:
		for (i = selected_.begin(); i != selected_.end(); i++) {
			Slot* slot = *i;
			slot->onMouseEvent( mouseListener, mEvent );
		}
		break;	
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

int Backpack::getWidth() const
{
	int slotsWidth = width_ * Slot::texture->getWidth();
	int spacingWidth = (width_ - 1) * (int)SLOT_SPACING;
	return ( slotsWidth + spacingWidth );
}

int Backpack::getHeight() const
{
	int slotsHeight = height_ * Slot::texture->getHeight();
	int spacingHeight = (height_ - 1) * (int)SLOT_SPACING;
	return ( slotsHeight + spacingHeight );
}

void Backpack::setLoaded()
{
	isLoaded_ = true;
}

bool Backpack::isLoaded() const
{
	return isLoaded_;
}