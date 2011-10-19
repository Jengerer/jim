#include "horizontal_layout.h"

HorizontalLayout::HorizontalLayout( unsigned int spacing, EVerticalAlignType alignType ) : Layout( spacing )
{
	// HorizontalLayout created.
	SetAlignType( alignType );
	SetMinimumHeight( 0 );
}

void HorizontalLayout::Pack( void )
{
	// First get maximum height.
	int maxHeight = 0;
	for (auto i = components_.begin(); i != components_.end(); i++) {
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
	for (auto i = components_.begin(); i != components_.end(); i++) {
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

		SetConstraint( component, width, posY );

		// Push width by component width and spacing (if not last).
		width += static_cast<float>(component->GetWidth());
		if ( component != components_.back() ) {
			width += static_cast<float>(GetSpacing());
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