#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "KeyboardHandler.h"
#include "IMouseHandler.h"
#include "Container.h"
#include "DirectX.h"
#include "Window.h"
#include "Mouse.h"

using namespace std;

enum EApplicationState {
	APPLICATION_STATE_START,
	APPLICATION_STATE_RUN,
	APPLICATION_STATE_EXIT
};

#define NUM_KEYCODES 255

// Main class that's the base of all applications.
class Application: public Container, public KeyboardHandler, public IMouseHandler
{
public:
	Application( int width, int height );
	virtual ~Application( void );

	virtual void	LoadInterfaces( const char* title, HINSTANCE instance );
	virtual void	CloseInterfaces( void );
	void			ExitApplication( void );

	// Drawing functions.
	void			DrawFrame( void );
	Window*			GetWindow( void ) const;

	// Sets the state of the application.
	void				SetState( EApplicationState state );
	EApplicationState	GetState( void ) const;

	// Main running functions.
	virtual void	RunApplication( void );

	// Input handling.
	virtual void	HandleKeyboard() = 0;

private:

	// Only application should be able to update mouse.
	void			UpdateMouse( void );

protected:

	DirectX		*directX_;
	Mouse		*mouse_;

private:

	EApplicationState	state_;

};