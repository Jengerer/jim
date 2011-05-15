#include "HorizontalLayout.h"

HorizontalLayout::HorizontalLayout( void )
{
	// HorizontalLayout created.
}

HorizontalLayout::~HorizontalLayout( void )
{
	// HorizontalLayout destroyed.
}

void HorizontalLayout::Pack( void )
{
	// First get maximum height.
	int maxHeight = 0;
	deque<Component*>::const_iterator i;
	for (i = componentStack_.begin(); i != componentStack_.end(); i++) {
		Component *component = *i;
		int height = component->GetHeight();
		if (height > maxHeight) {
			maxHeight = height;
		}
	}

	// Now pack.
	int width = 0;
	for (i = componentStack_.begin(); i != componentStack_.end(); i++) {
		Component *component = *i;

		// Set position aligned vertically.
		int middleY = GetY() + ((maxHeight - component->GetHeight()) >> 1);
		component->SetPosition( GetX() + width, middleY );

		// Push width by component width and spacing (if not last).
		width += component->GetWidth();
		if ( component != componentStack_.back() ) {
			width += GetSpacing();
		}
	}

	// Update size.
	SetSize( width, maxHeight );
}

void HorizontalLayout::UpdatePosition( void )
{
	Pack();
}