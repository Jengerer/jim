#include "Backpack.h"

const float	SLOT_SPACING = 5.0f;

Backpack::Backpack(
	float x, float y,
	int width, int height,
	int pages,
	Component* parent ) : Inventory( width, height, pages ), Container( x, y )
{
	// Backpack was created.
	isLoaded_ = false;
	addMouseListener( this ); // TODO: May not need.

	// Move to start.
	setPosition( x, y );
}

Backpack::~Backpack()
{
	// Backpack has been destroyed.
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

void Backpack::setPosition( float x, float y )
{
	const slotVector* inventory = getInventory();
	slotVector::const_iterator i;
	for (i = inventory->begin(); i != inventory->end(); i++) {
		Slot* slot = *i;

		int index = slot->getIndex();
		int x = index % width_;
		int y = index / height_;

		if (y >= height_) {
			x += getWidth() * (y / width_);
			y %= height_;
		}
		
		// Set position.
		float slotX = getX() + x*(Slot::texture->getWidth() + SLOT_SPACING);
		float slotY = getY() + y*(Slot::texture->getHeight() + SLOT_SPACING);
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

void Backpack::mouseClicked( Mouse *mouse, Component *component )
{
	// Mouse clicked.
}

void Backpack::mouseReleased( Mouse *mouse, Component *component )
{
	// Mouse released.
}

void Backpack::mouseMoved( Mouse *mouse, Component *component )
{
	// Mouse moved.
}

int Backpack::getWidth() const
{
	// HACK: Need to get actual size.
	return 795;
}

int Backpack::getHeight() const
{
	// HACK: Need to get actual size.
	return 0;
}

void Backpack::setLoaded()
{
	isLoaded_ = true;
}

bool Backpack::isLoaded() const
{
	return isLoaded_;
}