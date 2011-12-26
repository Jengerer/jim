#include "item_manager.h"
#include <jui/application_controller.h>

// Global application handle.
int WINAPI WinMain( HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd )
{
	// Create instance of application.
	ApplicationController::initialize( new ItemManager( instance ) );

	// Run application main loop.
	ApplicationController::main_loop();

	// Exit application after main loop.
	ApplicationController::exit();

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return EXIT_SUCCESS;
}