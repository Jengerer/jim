#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "keyboard_handler.h"
#include "imouse_handler.h"
#include "container.h"
#include "directx.h"
#include "window.h"
#include "mouse.h"

using namespace std;

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

};