#include "horizontal_layout.h"

HorizontalLayout::HorizontalLayout( unsigned int spacing, EVerticalAlignType alignType ) : Layout( spacing )
{
	// HorizontalLayout created.
	set_align_type( alignType );
	SetMinimumHeight( 0 );
}

void HorizontalLayout::pack( void )
{
	// First get maximum height.
	int maxHeight = 0;
	for (auto i = components_.begin(); i != components_.end(); i++) {
		Component *component = *i;
		int height = component->get_height();
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
		switch (get_align_type()) {
		case ALIGN_TOP:
			posY = 0.0f;
			break;
		case ALIGN_MIDDLE:
			posY = static_cast<float>(maxHeight - component->get_height()) / 2.0f;
			break;
		case ALIGN_BOTTOM:
			posY += static_cast<float>(maxHeight - component->get_height());
			break;
		}

		set_constraint( component, width, posY );

		// Push width by component width and spacing (if not last).
		width += static_cast<float>(component->get_width());
		if ( component != components_.back() ) {
			width += static_cast<float>(GetSpacing());
		}
	}

	// Update size.
	set_size( static_cast<int>(width), maxHeight );
}

void HorizontalLayout::SetMinimumHeight( int minimumHeight )
{
	minimumHeight_ = minimumHeight;
}

void HorizontalLayout::set_align_type( EVerticalAlignType alignType )
{
	alignType_ = alignType;
}

EVerticalAlignType HorizontalLayout::get_align_type( void ) const
{
	return alignType_;
}