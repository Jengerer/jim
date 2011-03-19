#pragma once

#include <vector>

#include "Component.h"
#include "Container.h"

// Abstract class to format elements within a container.
class Layout: public Container
{
public:
	Layout();
	virtual ~Layout();

	// Format the elements into the appropriate layout.
	virtual void	pack() = 0;
	virtual void	updatePosition() = 0;

	// Sets the spacing between elements.
	int				getSpacing() const;
	void			setSpacing( int spacing );

	// Bound restriction based on parent.
	virtual bool withinBounds( Component *component );

	// Sets a parent to handle size constraints.
	void			setParent( Container *parent );
	Container*		getParent();

private:
	Container	*parent_;
	int			spacing_;
};