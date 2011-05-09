#pragma once

#include <deque>
#include "Component.h"

class Component;
class Container : public Component
{
public:
	Container( float x = 0.0f, float y = 0.0f );
	virtual ~Container( void );

	// Container resource handling.
	void				Add( Component *component );
	void				AddBottom( Component *component );
	void				Remove( Component *component );
	void				RemoveAll( void );
	deque<Component*>*	GetChildren( void );

	virtual void		OnDraw( DirectX *directX );

	// Relative position modifiers.
	virtual void		SetPosition( float x, float y );
	virtual void		UpdatePosition( void );
	virtual bool		IsVisible( Component *component ) const;
	virtual bool		WithinBounds( Component *component ) const;
	void				ClampChild( Component *component, int padding = 0 ) const;

protected:

	deque<Component*>	componentStack_;

};