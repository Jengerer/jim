#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "curl.h"

using std::cout;

int main( int argc, char** argv )
{
	cout << "Welcome to the Inconvenient Item Manager Update System!\n\n";
	cout << "Euthenizing item_manager.exe... ";

	try {
		// Terminate application.
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

	Curl* curl = nullptr;
	try {
		curl = Curl::get_instance();
		cout << "SUCCESS!\n";

		cout << "Downloading item_manager.exe... ";
		curl->download( "http://www.jengerer.com/item_manager/item_manager.exe", "item_manager.exe" );
		cout << "SUCCESS!\n";

		cout << "Downloading steam_api.dll... ";
		curl->download( "http://www.jengerer.com/item_manager/steam_api.dll", "steam_api.dll" );
		cout << "SUCCESS!\n";

		cout << "Downloading libpng15.dll... ";
		curl->download( "http://www.jengerer.com/item_manager/libpng15.dll", "libpng15.dll" );
		cout << "SUCCESS!\n";
	}
	catch (const std::runtime_error&) {
		cout << "FAILED!\n";
		Curl::shut_down();
		system( "pause" );
		return EXIT_FAILURE;
	}

	cout << "\n";
	cout << "Wow, nothing broke, that's a surprise.\n";
	cout << "Launching item manager after superfluous 3 second pause...\n";

	Sleep( 3000 );
	ShellExecute( 0, 0, "item_manager.exe", 0, 0, SW_SHOWDEFAULT );

	return EXIT_SUCCESS;
}