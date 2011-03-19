#include "Panel.h"

Panel::Panel()
{
	// Panel constructed.
}

Panel::Panel( float x, float y ) : Container( x, y )
{
	// Panel constructed.
}

Panel::~Panel()
{
	// Panel deconstructed.
}

void Panel::draw( DirectX *directX )
{
	for (int i = 0; i < componentStack_.size(); i++) {
		Component *component = componentStack_.at( i );
		component->draw( directX );
	}
}