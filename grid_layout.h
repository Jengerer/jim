#ifndef GRID_LAYOUT_H
#define GRID_LAYOUT_H

#include "layout.h"

class GridLayout : public Layout
{

public:

	GridLayout( int gridWidth, unsigned int spacing );
	virtual ~GridLayout( void );

	virtual void pack( void );

private:

	int gridWidth_;

};

#endif // GRID_LAYOUT_H