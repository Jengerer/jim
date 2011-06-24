#include "vertical_layout.h"

VerticalLayout::VerticalLayout( unsigned int spacing, EHorizontalAlignType alignType ) : Layout( spacing )
{
	SetAlignType( alignType );
	SetMinimumWidth( 0 );
}

void VerticalLayout::Pack( void )
{
	// First get maximum width.
	int maxWidth = 0;
	deque<Component*>::const_iterator i, end;
	for (i = componentStack_.begin(), end = componentStack_.end(); i != end; ++i) {
		Component *component = *i;
		int width = component->GetWidth();
		if (width > maxWidth) {
			maxWidth = width;
		}
	}

	if (maxWidth < minimumWidth_) {
		maxWidth = minimumWidth_;
	}

	// Now pack.
	int height = 0;
	for (i = componentStack_.begin(); i != end; i++) {
		Component *component = *i;

		// Set position aligned horizontally.
		float posX = 0.0f;
		switch (GetAlignType()) {
		case ALIGN_LEFT:
			break;
		case ALIGN_CENTER:
			posX = ((maxWidth - component->GetWidth()) >> 1);
			break;
		case ALIGN_RIGHT:
			posX = (maxWidth - component->GetWidth());
			break;
		}
		component->SetLocalPosition( posX, height );

		// Push width by component width and spacing (if not last).
		height += component->GetHeight();
		if ( component != componentStack_.back() ) {
			height += GetSpacing();
		}
	}

	// Update size.
	SetSize( maxWidth, height );
	UpdateChildren();
}

void VerticalLayout::SetAlignType( EHorizontalAlignType alignType )
{
	alignType_ = alignType;
}

EHorizontalAlignType VerticalLayout::GetAlignType( void ) const
{
	return alignType_;
}

void VerticalLayout::SetMinimumWidth( int minimumWidth )
{
	minimumWidth_ = minimumWidth;
}