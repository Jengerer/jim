#include "Draggable.h"

Draggable::Draggable( MouseListener* mouseListener,
	float x, float y ) : Drawable( x, y )
{
	mouseListener_ = mouseListener;
}

