#ifndef WINDOW_H
#define WINDOW_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdexcept>

class Window
{

public:

	//Class constructor.
	Window( HINSTANCE hInstance );
	~Window();

	// Window creation.
	void		create_window();
	
	// Get window external parameters.
	HWND		get_handle() const;
	const char* get_title() const;

	// Setting window display parameters.
	void		set_title( const char* title );
	void		set_size( int width, int height );
	void		set_fullscreen( bool is_fullscreen );
	void		set_border( bool has_border );

	// Get window display parameters.
	int			get_width() const;
	int			get_height() const;
	bool		is_fullscreen() const;
	bool		has_border() const;

	// Window status parameters.
	bool		is_active();

private:

	// Creating window class.
	bool	register_class();

	// Setting window external parameters.
	void		set_handle( HWND hwnd );
	void		set_instance( HINSTANCE instance );
	HINSTANCE	get_instance() const;

private:

	// Window handle.
	HWND		wnd_;

	// Window external parameters.
	const char*	title_;
	HINSTANCE	instance_;

	// Window display parameters.
	int			width_;
	int			height_;
	bool		is_fullscreen_;
	bool		has_border_;

};

#endif // WINDOW_H