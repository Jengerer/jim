#include "item_manager.h"

// Global application handle.
static Application* application = nullptr;

/*
 * Handles window messages.
 */
LRESULT CALLBACK wnd_proc( HWND wnd, UINT message, WPARAM wparam, LPARAM lparam )
{
	switch (message) {
	case WM_KEYDOWN:
		application->trigger_key_pressed( LOWORD( wparam ) );
		break;

	case WM_KEYUP:
		application->trigger_key_released( LOWORD( wparam ) );
		break;

	case WM_LBUTTONDOWN:
		application->trigger_mouse_clicked();
		break;

	case WM_LBUTTONUP:
		application->trigger_mouse_released();
		break;

	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
		break;
	}

	return DefWindowProc( wnd, message, wparam, lparam );
}

int WINAPI WinMain( HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd )
{
	// Create instance of application.
	application = new ItemManager( instance );

	// Load application interfaces.
	try {
		application->load_interfaces();
	}
	catch (const std::runtime_error& ex) {
		MessageBox( NULL, ex.what(), "Initialization failed!", MB_OK );
	}

	// Enter main program loop.
	MSG msg;
	bool running = true;
	while (running) {
		// Check for Windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				running = false;
			}
			else {
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}

		// Run frame.
		application->run();
	}

	// Clean application.
	delete application;
	application = nullptr;

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return EXIT_SUCCESS;
}