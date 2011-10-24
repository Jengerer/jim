#ifndef IDRAWABLE_H
#define IDRAWABLE_H

#include "graphics_2d.h"

class IDrawable
{

public:

	virtual void draw( Graphics2D* graphics ) = 0;

};

#endif // IDRAWABLE_H