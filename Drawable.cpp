#include "Drawable.h"

Drawable::Drawable( float x, float y ) : Component( x, y )
{
	// Visible by default.
	alpha = 100;
}