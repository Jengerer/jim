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
	float width = 0.0f;
	for (i = componentStack_.begin(); i != componentStack_.end(); i++) {
		Component *component = *i;

		// Set position aligned vertically.
		float posY;
		switch (GetAlignType()) {
		case ALIGN_TOP:
			posY = 0.0f;
			break;
		case ALIGN_MIDDLE:
			posY = static_cast<float>(maxHeight - component->GetHeight()) / 2.0f;
			break;
		case ALIGN_BOTTOM:
			posY += static_cast<float>(maxHeight - component->GetHeight());
			break;
		}
		component->SetLocalPosition( width, posY );

		// Push width by component width and spacing (if not last).
		width += static_cast<float>(component->GetWidth());
		if ( component != componentStack_.back() ) {
			width += static_cast<float>(GetSpacing());
		}
	}

	// Update size.
	SetSize( width, maxHeight );
	UpdateChildren();
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