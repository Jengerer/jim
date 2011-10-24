#include "layout.h"

Layout::Layout( unsigned int spacing )
{
	// Layout created.
	set_parent( nullptr );
	SetSpacing( spacing );
}

Layout::~Layout( void )
{
	// Layout destroyed.
}

bool Layout::IsVisible( Component *component ) const
{
	const Container *parent = get_parent();
	if ( parent != nullptr ) {
		return parent->IsVisible( component );
	}

	return Container::IsVisible( component );
}

void Layout::SetSpacing( int spacing )
{
	spacing_ = spacing;
}

int Layout::GetSpacing( void ) const
{
	return spacing_;
}

void Layout::set_parent( Container *parent )
{
	parent_ = parent;
}

const Container* Layout::get_parent( void ) const
{
	return parent_;
}