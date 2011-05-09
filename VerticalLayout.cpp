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
	int width = 0, height = 0;
	deque<Component*>::iterator i;
	for (i = componentStack_.begin(); i != componentStack_.end(); i++) {
		Component *component = *i;
		component->SetPosition( GetX(), GetY() + height );

		// Push height by component height and spacing (if not last).
		height += component->GetHeight();
		if (component != componentStack_.back()) {
			height += GetSpacing();
		}

		// Store maximum width.
		if (component->GetWidth() > width) {
			width = component->GetWidth();
		}
	}

	SetSize( width, height );
}

void VerticalLayout::UpdatePosition( void )
{
	Pack();
}