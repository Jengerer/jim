#include "item_manager.hpp"
#include <jui/application_controller.hpp>

// Global application handle.
int WINAPI WinMain( HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd )
{
	// Create instance of application.
	bool success = ApplicationController::initialize( new ItemManager( instance ) );
	if (success) {
		ApplicationController::main_loop();
	}

	// Exit application after main loop.
	ApplicationController::exit();

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return EXIT_SUCCESS;
}