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
	int width = 0, height = 0;
	deque<Component*>::iterator i;
	for (i = componentStack_.begin(); i != componentStack_.end(); i++) {
		Component *component = *i;
		component->SetPosition( GetX() + width, GetY() );

		// Push width by component width and spacing (if not last).
		width += component->GetWidth();
		if ( component != componentStack_.back() ) {
			width += GetSpacing();
		}

		// Store maximum height.
		if (component->GetHeight() > height) {
			height = component->GetHeight();
		}
	}

	SetSize( width, height );
}

void HorizontalLayout::UpdatePosition( void )
{
	Pack();
}