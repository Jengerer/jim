#include "HorizontalLayout.h"

HorizontalLayout::HorizontalLayout()
{
	// HorizontalLayout created.
}

HorizontalLayout::~HorizontalLayout()
{
	// HorizontalLayout destroyed.
}

void HorizontalLayout::pack()
{
	int width = 0, height = 0;
	deque<Component*>::iterator i;
	for (i = componentStack_.begin(); i != componentStack_.end(); i++) {
		Component *component = *i;
		component->setPosition( getX() + width, getY() );

		// Push width by component width and spacing (if not last).
		width += component->getWidth();
		if (component != componentStack_.back()) {
			width += getSpacing();
		}

		// Store maximum height.
		if (component->getHeight() > height) {
			height = component->getHeight();
		}
	}

	setSize( width, height );
}

void HorizontalLayout::updatePosition() {
	pack();
}