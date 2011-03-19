#include "Layout.h"

Layout::Layout()
{
	// Layout created.
	setParent( 0 );
	setSpacing( 0 );
}

Layout::~Layout()
{
	// Layout destroyed.
}

bool Layout::withinBounds( Component *component )
{
	if (getParent() != 0) {
		return getParent()->withinBounds( component );
	}

	return Container::withinBounds( component );
}

void Layout::setSpacing( int spacing )
{
	spacing_ = spacing;
}

int Layout::getSpacing() const
{
	return spacing_;
}

void Layout::setParent( Container *parent )
{
	parent_ = parent;
	if (parent_ != 0) {
		setSize( parent_->getWidth(), parent_->getHeight() );
	}
}

Container* Layout::getParent()
{
	return parent_;
}