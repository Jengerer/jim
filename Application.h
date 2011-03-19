#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "KeyboardHandler.h"
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
class Application: public Container, public KeyboardHandler
{
public:
	Application( const char* title,
		HINSTANCE hInstance,
		int width,
		int height );
	virtual ~Application();

	virtual void	openInterfaces() = 0;
	virtual void	closeInterfaces() = 0;
	void			exitApplication();

	// Drawing functions.
	virtual void	onRedraw();
	void			redraw();
	Window*			getWindow() const;

	// Sets the state of the application.
	void			setState( EApplicationState state );

	// Returns the state of the application.
	EApplicationState		getState() const;

	// Component size getters.
	virtual int		getWidth() const;
	virtual int		getHeight() const;

	// Main running functions.
	virtual void	run() = 0;

	// Input handling.
	void			handleMouse();
	virtual void	handleKeyboard() = 0;

protected:
	DirectX		*directX_;
	Mouse		*mouse_;

	// Cursors.
	HCURSOR		arrow_;
	HCURSOR		hand_;
	HCURSOR		drag_;

private:
	EApplicationState	state_;
};