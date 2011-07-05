#include "grid_layout.h"

GridLayout::GridLayout( int gridWidth, unsigned int spacing )
{
	gridWidth_ = gridWidth;
	SetSpacing( spacing );
}

GridLayout::~GridLayout( void )
{
	// GridLayout destroyed.
}

void GridLayout::Pack( void )
{
	if (components_.empty()) {
		SetSize( 0, 0 );
		return;
	}

	// Base component sizes on first element.
	Component *first = components_.front();
	int componentWidth = first->GetWidth();
	int componentHeight = first->GetHeight();
	int spacing = GetSpacing();

	// Grid height is components / width, + 1 if remainder.
	size_t numComponents = components_.size();
	int gridHeight = numComponents / gridWidth_ + (numComponents % gridWidth_ == 0 ? 0 : 1);

	// Calculate width and height.
	int totalWidth = gridWidth_ * (componentWidth + spacing) - spacing;
	int totalHeight = gridHeight * (componentHeight + spacing) - spacing;
	SetSize( totalWidth, totalHeight );

	size_t index = 0;
	float x, y;
	std::vector<Component*>::iterator i, end;
	for (i = components_.begin(), end = components_.end(); i != end; ++i) {
		Component *current = *i;
		x = static_cast<float>((index % gridWidth_) * (componentWidth + spacing));
		y = static_cast<float>((index / gridWidth_) * (componentHeight + spacing));
		SetConstraint( current, x, y );

		++index;
	}
}