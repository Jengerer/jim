#include "VerticalLayout.h"

VerticalLayout::VerticalLayout( void )
{
	// VerticalLayout created.
}

VerticalLayout::~VerticalLayout( void )
{
	// VerticalLayout destroyed.
}

void VerticalLayout::Pack( void )
{
	// First get maximum width.
	int maxWidth = 0;
	deque<Component*>::const_iterator i;
	for (i = componentStack_.begin(); i != componentStack_.end(); i++) {
		Component *component = *i;
		int width = component->GetWidth();
		if (width > maxWidth) {
			maxWidth = width;
		}
	}

	// Now pack.
	int height = 0;
	for (i = componentStack_.begin(); i != componentStack_.end(); i++) {
		Component *component = *i;

		// Set position aligned horizontally.
		int middleX = GetX() + ((maxWidth - component->GetWidth()) >> 1);
		component->SetPosition( middleX, GetY() + height );

		// Push width by component width and spacing (if not last).
		height += component->GetHeight();
		if ( component != componentStack_.back() ) {
			height += GetSpacing();
		}
	}

	// Update size.
	SetSize( maxWidth, height );
}

void VerticalLayout::UpdatePosition( void )
{
	Pack();
}