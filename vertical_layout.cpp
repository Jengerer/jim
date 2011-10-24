#include "vertical_layout.h"

VerticalLayout::VerticalLayout( unsigned int spacing, EHorizontalAlignType alignType ) : Layout( spacing )
{
	set_align_type( alignType );
	set_minimum_width( 0 );
}

void VerticalLayout::pack( void )
{
	// First get maximum width.
	int maxWidth = 0;
	std::vector<Component*>::const_iterator i, end;
	for (i = components_.begin(), end = components_.end(); i != end; ++i) {
		Component *component = *i;
		int width = component->get_width();
		if (width > maxWidth) {
			maxWidth = width;
		}
	}

	if (maxWidth < minimumWidth_) {
		maxWidth = minimumWidth_;
	}

	// Now pack.
	int height = 0;
	for (i = components_.begin(); i != end; i++) {
		Component *component = *i;

		// Set position aligned horizontally.
		float posX;
		switch (get_align_type()) {
		case ALIGN_LEFT:
			posX = 0.0f;
			break;
		case ALIGN_CENTER:
			posX = static_cast<float>(maxWidth - component->get_width()) / 2.0f;
			break;
		case ALIGN_RIGHT:
			posX = static_cast<float>(maxWidth - component->get_width());
			break;
		}

		set_constraint( component, posX, static_cast<float>(height) );

		// Push width by component width and spacing (if not last).
		height += component->get_height();
		if ( component != components_.back() ) {
			height += GetSpacing();
		}
	}

	// Update size.
	set_size( maxWidth, height );
}

void VerticalLayout::set_align_type( EHorizontalAlignType alignType )
{
	alignType_ = alignType;
}

EHorizontalAlignType VerticalLayout::get_align_type( void ) const
{
	return alignType_;
}

void VerticalLayout::set_minimum_width( int minimumWidth )
{
	minimumWidth_ = minimumWidth;
}