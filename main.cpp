#include "item_manager.hpp"
#include <jui/application/application_controller.hpp>

// Global application handle.
int WINAPI WinMain( HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd )
{
#ifdef _DEBUG
    OutputDebugString( "== BEFORE INITIALIZE ==\n" );
    _CrtDumpMemoryLeaks();
#endif
    // Start in scope so manager is destroyed.
    {
	    bool success = JUI::ApplicationController::initialize<ItemManager>( instance );
	    if (success) {
		    JUI::ApplicationController::main_loop();
	    }

	    // Exit application after main loop.
	    JUI::ApplicationController::exit();
    }

#ifdef _DEBUG
    OutputDebugString( "== AFTER INITIALIZE ==\n" );
	_CrtDumpMemoryLeaks();
#endif

	return EXIT_SUCCESS;
}