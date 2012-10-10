#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

#include <jui/net/file_downloader.hpp>
#include <jui/application/application.hpp>

// Filenames for updating.
const JUTIL::ConstantString item_manager_file( "item_manager.exe" );
const JUTIL::ConstantString item_manager_url( "http://www.jengerer.com/item_manager/item_manager.exe" );
const JUTIL::ConstantString steam_api_file( "steam_api.dll" );
const JUTIL::ConstantString steam_api_url( "http://www.jengerer.com/item_manager/steam_api.dll" );
const JUTIL::ConstantString libpng_file( "libpng15.dll" );
const JUTIL::ConstantString libpng_url( "http://www.jengerer.com/item_manager/libpng15.dll" );

using std::cout;

int main( int argc, char** argv )
{
	cout << "Welcome to the Inconvenient Item Manager Update System!\n\n";
	cout << "Euthenizing item_manager.exe... ";
	
	try {
		// Terminate running manager.
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof( PROCESSENTRY32 );
		HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
		if (Process32First( snapshot, &entry )) {
			while (Process32Next( snapshot, &entry )) {
				if (strcmp( entry.szExeFile, "item_manager.exe" ) == 0) {
					HANDLE process = OpenProcess( PROCESS_ALL_ACCESS, false, entry.th32ProcessID );
					if (process != nullptr) {
						// Check if it'll close itself.
						const DWORD TIMEOUT = 2500;
						DWORD result = WaitForSingleObject( process, TIMEOUT );
						if (result == WAIT_TIMEOUT) {
							if (!TerminateProcess( process, 0 )) {
								CloseHandle( process );
								throw std::runtime_error( "Failed to terminate process." );
							}
						}
						else if (result != WAIT_OBJECT_0) {
							CloseHandle( process );
							throw std::runtime_error( "Failed to wait for object." );
						}

						CloseHandle( process );
					}
				}
			}
		}
	}
	catch (const std::runtime_error& ex) {
		cout << "FAILED!\n";
		cout << ex.what() << "\n";
		system( "pause" );
		return EXIT_FAILURE;
	}

	cout << "EUTHENIZED!\n\n";
	cout << "Preparing update interfaces... ";

	// Start downloading files.
	try {
		// Set up downloader.
		cout << "SUCCESS!\n";
		JUI::FileDownloader* loader = JUI::FileDownloader::get_instance();

		// Download item manager executable.
		cout << "Downloading item_manager.exe... ";
		loader->get( &item_manager_file, &item_manager_url );
		cout << "SUCCESS!\n";

		// Download Steam API DLL.
		cout << "Downloading steam_api.dll... ";
		loader->check_and_get( &steam_api_file, &steam_api_url );
		cout << "SUCCESS!\n";

		// Download libpng15.dll if missing.
		cout << "Downloading libpng15.dll... ";
		loader->check_and_get( &libpng_file, &libpng_url );
		cout << "SUCCESS!\n";
	}
	catch (const std::runtime_error& ex) {
		// Close downloader on fail.
		JUI::FileDownloader::shut_down();

		// Print error.
		cout << "FAILED: " << ex.what() << "\n";
		system( "pause" );
		return EXIT_FAILURE;
	}

	// Close downloader.
	JUI::FileDownloader::shut_down();

	// Print last output.
	cout << "\n";
	cout << "Wow, nothing broke, that's a surprise.\n";
	cout << "Launching item manager after superfluous 3 second pause...\n";

	// Run item manager after pause.
	Sleep( 3000 );
	ShellExecute( 0, 0, "item_manager.exe", 0, 0, SW_SHOWDEFAULT );

	return EXIT_SUCCESS;
}