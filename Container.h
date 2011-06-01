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
	virtual void		Add( Component *component );
	virtual void		AddBottom( Component *component );
	virtual void		Remove( Component *component );
	virtual void		RemoveAll( void );
	deque<Component*>*	GetChildren( void );

	virtual void		OnDraw( DirectX *directX );
	virtual void		Pack( void );
	virtual void		SetGlobalPosition( float globalX, float globalY );
	void				UpdatePosition( void );
	virtual void		SetAlpha( int alpha );

	// Relative position modifiers.
	virtual bool		IsVisible( Component *component ) const;
	virtual bool		WithinBounds( Component *component ) const;
	void				ClampChild( Component *component, int padding = 0 ) const;

protected:

	deque<Component*>	componentStack_;

};