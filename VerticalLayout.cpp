#include "VerticalLayout.h"

VerticalLayout::VerticalLayout()
{
	// VerticalLayout created.
}

VerticalLayout::~VerticalLayout()
{
	// VerticalLayout destroyed.
}

void VerticalLayout::pack()
{
	int width = 0, height = 0;
	deque<Component*>::iterator i;
	for (i = componentStack_.begin(); i != componentStack_.end(); i++) {
		Component *component = *i;
		component->setPosition( getX(), getY() + height );

		// Push height by component height and spacing (if not last).
		height += component->getHeight();
		if (component != componentStack_.back()) {
			height += getSpacing();
		}

		// Store maximum width.
		if (component->getWidth() > width) {
			width = component->getWidth();
		}
	}

	setSize( width, height );
}

void VerticalLayout::updatePosition() {
	pack();
}