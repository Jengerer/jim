#ifndef SINGLE_CONTAINER_H
#define SINGLE_CONTAINER_H

class SingleContainer : public Component
{

public:

	SingleContainer( float x = 0.0f, float y = 0.0f );
	virtual ~SingleContainer( void );

	// SingleContainer resource handling.
	void	SetChild( Component *component );
	void	RemoveChild( void );

#endif // SINGLE_CONTAINER_H