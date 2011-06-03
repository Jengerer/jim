#include "GridLayout.h"

GridLayout::GridLayout( int gridWidth )
{
	gridWidth_ = gridWidth;
}

GridLayout::~GridLayout( void )
{
	// GridLayout destroyed.
}

void GridLayout::Pack( void )
{
	if (componentStack_.empty()) {
		SetSize( 0, 0 );
		return;
	}

	// Base component sizes on first element.
	Component *first = componentStack_.front();
	int componentWidth = first->GetWidth();
	int componentHeight = first->GetHeight();
	int spacing = GetSpacing();

	// Grid height is components / width, + 1 if remainder.
	size_t numComponents = componentStack_.size();
	int gridHeight = numComponents / gridWidth_ + (numComponents % gridWidth_ == 0 ? 0 : 1);

	// Calculate width and height.
	int totalWidth = gridWidth_ * (componentWidth + spacing) - spacing;
	int totalHeight = gridHeight * (componentHeight + spacing) - spacing;
	SetSize( totalWidth, totalHeight );

	size_t index = 0;
	float x, y;
	std::deque<Component*>::iterator i, end;
	for (i = componentStack_.begin(), end = componentStack_.end(); i != end; ++i) {
		Component *current = *i;
		x = static_cast<float>((index % gridWidth_) * (componentWidth + spacing));
		y = static_cast<float>((index / gridWidth_) * (componentHeight + spacing));
		current->SetLocalPosition( x, y );

		++index;
	}

	UpdateChildren();
}