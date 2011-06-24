#pragma once

#include <vector>

#include "component.h"
#include "container.h"

// Abstract class to format elements within a container.
class Layout: public Container
{

public:

	Layout( unsigned int spacing = 0 );
	virtual ~Layout( void );

	// Format the elements into the appropriate layout.
	virtual void		Pack( void ) = 0;

	// Sets the spacing between elements.
	int					GetSpacing( void ) const;
	void				SetSpacing( int spacing );

	// Bound restriction based on parent.
	virtual bool		IsVisible( Component *component ) const;

	// Sets a parent to handle size constraints.
	void				SetParent( Container *parent );
	const Container*	GetParent( void ) const ;

private:

	Container		*parent_;
	int				spacing_;

};