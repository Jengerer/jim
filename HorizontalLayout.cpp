#include "HorizontalLayout.h"

HorizontalLayout::HorizontalLayout( void )
{
	// HorizontalLayout created.
	SetMinimumHeight( 0 );
	SetAlignType( ALIGN_MIDDLE );
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

	if (maxHeight < minimumHeight_) {
		maxHeight = minimumHeight_;
	}

	// Now pack.
	int width = 0;
	for (i = componentStack_.begin(); i != componentStack_.end(); i++) {
		Component *component = *i;

		// Set position aligned vertically.
		int posY = GetGlobalY();
		switch (GetAlignType()) {
		case ALIGN_TOP:
			break;
		case ALIGN_MIDDLE:
			posY += ((maxHeight - component->GetHeight()) >> 1);
			break;
		case ALIGN_BOTTOM:
			posY += (maxHeight - component->GetHeight());
			break;
		}
		component->SetLocalPosition( width, posY );

		// Push width by component width and spacing (if not last).
		width += component->GetWidth();
		if ( component != componentStack_.back() ) {
			width += GetSpacing();
		}
	}

	// Update size.
	SetSize( width, maxHeight );
}

void HorizontalLayout::SetMinimumHeight( int minimumHeight )
{
	minimumHeight_ = minimumHeight;
}

void HorizontalLayout::SetAlignType( EVerticalAlignType alignType )
{
	alignType_ = alignType;
}

EVerticalAlignType HorizontalLayout::GetAlignType( void ) const
{
	return alignType_;
}