#ifndef APPLICATION_H
#define APPLICATION_H

#include "container.h"
#include "graphics_2d.h"
#include "imouse_handler.h"
#include "ikeyboard_handler.h"
#include "keyboard.h"
#include "mouse.h"
#include "window.h"

// Main class that's the base of all applications.
class Application: public Container, public IMouseHandler, public IKeyboardHandler
{
public:

	Application( HINSTANCE instance );
	virtual ~Application( void );

	// Create interfaces.
	virtual void	load_interfaces( void );
	virtual void	close_interfaces( void );
	void			exit_application( void );

	// Application singularity/dependency checks.
	static unsigned int get_process_count( const char* process_name );

	// Drawing functions.
	void			draw_frame( void );
	Window*			get_window( void ) const;

	// Main running functions.
	virtual void	run( void );

	// Input event triggers.
	void			trigger_mouse_events( void );
	void			trigger_mouse_moved( void );
	void			trigger_mouse_clicked( void );
	void			trigger_mouse_released( void );
	void			trigger_key_pressed( int key );
	void			trigger_key_released( int key );

protected:

	Graphics2D*		graphics_;
	Window*			window_;
	Mouse*			mouse_;
	Keyboard		keyboard_;

};

#endif // APPLICATION_H