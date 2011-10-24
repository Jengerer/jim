#ifndef HORIZONTAL_SPLIT_CONTAINER_H
#define HORIZONTAL_SPLIT_CONTAINER_H

#include "layout.h"
#include "horizontal_layout.h"

class HorizontalSplitLayout: public Layout
{

public:

	HorizontalSplitLayout( int width, unsigned int spacing = 0 );

	virtual void pack();
	void add_left( Component* component );
	void add_right( Component* component );

private:

	HorizontalLayout* left_;
	HorizontalLayout* right_;

};

#endif // HORIZONTAL_SPLIT_CONTAINER_H