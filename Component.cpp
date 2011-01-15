#include "Component.h"

Component::Component( float x, float y, EAlignment align )
{
	// Set defaults.
	setPosition( x, y, align );
	setMouseListener( 0 );
}

Component::~Component()
{
	// Component is destroyed.
}

void Component::setMouseListener( MouseListener* mouseListener )
{
	mouseListener_ = mouseListener;
}

bool Component::mouseEvent( Mouse *mouse, EMouseEvent eventType ) {
	// Generate mouse event function.
	if (mouse->isTouching( this )) {
		if (mouseListener_) {
			void (MouseListener::*mouseAction)( Mouse* mouse, Component* component );
			if (eventType == MOUSE_EVENT_MOVE) {
				mouseAction = &MouseListener::mouseMoved;
			}
			else if (eventType == MOUSE_EVENT_CLICK) {
				mouseAction = &MouseListener::mouseClicked;
			}
			else if (eventType == MOUSE_EVENT_RELEASE) {
				mouseAction = &MouseListener::mouseReleased;
			}
			else {
				throw Exception( "Unhandled mouse event." );
			}

			(mouseListener_->*mouseAction)( mouse, this );
		}

		return true;
	}

	return false;
}

void Component::setX( float x ) {
	x_ = x;
}

void Component::setY( float y ) {
	y_ = y;
}

void Component::setPosition( float x, float y, EAlignment align ) {
	setX( (align == ALIGN_TOP_LEFT || align == ALIGN_BOTTOM_LEFT ) ? x : x - getWidth() );
	setY( (align == ALIGN_TOP_LEFT || align == ALIGN_TOP_RIGHT ) ? y : y - getHeight() );
}

int Component::getWidth() const {
	return width_;
}

int Component::getHeight() const {
	return height_;
}

void Component::setSize( int width, int height ) {
	width_ = width;
	height_ = height;
}

float Component::getX() const {
	return x_;
}

float Component::getY() const {
	return y_;
}