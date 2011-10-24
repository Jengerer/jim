#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "keyboard.h"
#include "imouse_handler.h"
#include "container.h"
#include "graphics_2d.h"
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

	virtual void	load_interfaces( const char* title, HINSTANCE instance );
	virtual void	close_interfaces( void );
	void			exit_application( void );

	// Application singularity/dependency checks.
	static unsigned int get_process_count( const char* process_name );

	// Drawing functions.
	void			draw_frame( void );
	Window*			get_window( void ) const;

	// Main running functions.
	virtual void	run( void );

	// Input handling.
	virtual void	handle_keyboard() = 0;

private:

	// Only application should be able to update mouse.
	void			update_mouse( void );

protected:

	Graphics2D*	graphics_;
	Window*		window_;
	Mouse*		mouse_;

};