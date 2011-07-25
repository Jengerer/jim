#ifndef COLOUR_H
#define COLOUR_H

#include "opengl_shared.h"

// Colour struct.
#pragma pack( push, 1 )
struct Colour
{
	GLubyte r;
	GLubyte g;
	GLubyte b;
};
#pragma pack( pop )

// Default some useful colour constants.
const Colour COLOUR_WHITE = { 255, 255, 255 };
const Colour COLOUR_BLACK = { 0, 0, 0 };

// Alpha constants.
const GLubyte ALPHA_MINIMUM = 0;
const GLubyte ALPHA_MAXIMUM = 255;

#endif // COLOUR_H