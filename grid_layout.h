#ifndef GRID_LAYOUT_H
#define GRID_LAYOUT_H

#include "layout.h"

class GridLayout : public Layout
{

public:

	GridLayout( int gridWidth );
	virtual ~GridLayout( void );

	virtual void Pack( void );

private:

	int gridWidth_;

};

#endif // GRID_LAYOUT_H