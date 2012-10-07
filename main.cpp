#include "item_manager.hpp"
#include <jui/application/application_controller.hpp>

// Global application handle.
int WINAPI WinMain( HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd )
{
	ItemManager item_manager( instance );

	// Create instance of application.
	bool success = JUI::ApplicationController::initialize( &item_manager );
	if (success) {
		JUI::ApplicationController::main_loop();
	}

	// Exit application after main loop.
	JUI::ApplicationController::exit();

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return EXIT_SUCCESS;
}