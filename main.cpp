#include "item_manager.hpp"
#include <jui/application/application_controller.hpp>
#include <string/constant_string.hpp>
#include <dbghelp.h>
#include <Windows.h>

LONG WINAPI unhandled_exception_filter( PEXCEPTION_POINTERS exception_pointers )
{
	HANDLE dump_file = CreateFile("crash.dmp", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (dump_file != INVALID_HANDLE_VALUE) {
		MINIDUMP_EXCEPTION_INFORMATION info = {};
		info.ThreadId = GetCurrentThreadId();
		info.ExceptionPointers = exception_pointers;
		info.ClientPointers = TRUE;
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), dump_file, MiniDumpWithFullMemory, &info, nullptr, nullptr);
		CloseHandle(dump_file);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

// Global application handle.
int WINAPI WinMain( HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd )
{
	// Set exception handler
	SetUnhandledExceptionFilter(&unhandled_exception_filter);

	// Test it out!
	JUTIL::ConstantString cmd_line_string(cmd_line);
	const JUTIL::ConstantString fake_crash_option("-fakegpf");
	const JUTIL::ConstantString fake_debug_break_option("-fakedebugbreak");
	if (cmd_line_string.find(&fake_crash_option) != JUTIL::ConstantString::INVALID_INDEX)
	{
		*(int*)6 = 7;
	}
	else if (cmd_line_string.find(&fake_debug_break_option) != JUTIL::ConstantString::INVALID_INDEX)
	{
		__debugbreak();
	}

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