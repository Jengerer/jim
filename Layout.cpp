#include "Layout.h"

Layout::Layout( void )
{
	// Layout created.
	SetParent( nullptr );
	SetSpacing( 0 );
}

Layout::~Layout( void )
{
	// Layout destroyed.
}

bool Layout::IsVisible( Component *component ) const
{
	if ( GetParent() != nullptr ) {
		return parent_->IsVisible( component );
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

void Layout::SetParent( Container *parent )
{
	parent_ = parent;
}

const Container* Layout::GetParent( void ) const
{
	return parent_;
}