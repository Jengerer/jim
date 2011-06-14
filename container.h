#pragma once

#include <deque>
#include "Component.h"

class Container : public Component
{

public:

	Container( float localX = 0.0f, float localY = 0.0f );
	virtual ~Container( void );

	// Container resource handling.
	virtual void		Add( Component *component );
	virtual void		Remove( Component *component );
	virtual void		RemoveAll( void );
	deque<Component*>*	GetChildren( void );

	// Drawing functions.
	virtual void		OnDraw( DirectX *directX );
	virtual void		SetAlpha( int alpha );

	// Local/global child position handling.
	virtual void		Pack( void );
	void				UpdateChildren( void );
	void				UpdateChild( Component *child ) const;
	virtual void		SetGlobalPosition( float globalX, float globalY );
	void				ClampChild( Component *component, float padding = 0.0f ) const;
	virtual bool		IsVisible( Component *component ) const;
	virtual bool		WithinBounds( Component *component ) const;

protected:

	deque<Component*>	componentStack_;

};