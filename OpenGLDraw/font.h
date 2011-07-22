#ifndef FONT_H
#define FONT_H

#include "exception.h"
#include "opengl_shared.h"
#include <OGLFT.h>

class Font
{

public:

	Font( const string& fontPath, float size );

private:

	OGLFT::Monochrome* face_;
	
};

#endif // FONT_H