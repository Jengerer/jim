#include "AutoUpdater.h"

#include <windows.h>
#include <stdio.h>
#include <conio.h>

int main()
{
	printf( "=== Jengerer's Item Manager AutoUpdater ===\n\n" );

	printf( "Waiting for Jengerer's Item Manager to close...\n\n" );
	Sleep(2500);

	printf( "Initiating connection to update server...\n" );

	Curl* curl = 0;

	try {
		curl = new Curl();
	}
	catch (Exception curlException) {
		printf( "Failed to initialize cURL. Updating failed." );

		if (curl)
		{
			delete curl;
			curl = 0;
		}
	}

	printf( "Connection successfully initialized!\n\n" );
	printf( "Downloading updated files...\n\n" );

	bool bSucceeded = true;

	//Download the latest files.
	if (!curl->download( "http://www.jengerer.com/itemManager/ItemManager.exe", "ItemManager.exe" ) ||
		!curl->download( "http://www.jengerer.com/itemManager/steam_api.dll", "steam_api.dll" ))
	{
		printf( "Failed to download one or more files from the latest version of the item manager!\n" );
		printf( "Make sure the Item Manager isn't running, and try again.\n\n" );

		printf( "Press any key to close...\n" );

		_getch();
	} else
	{
		printf( "Update successfully completed!\n\n" );

		FILE* exeManager;
		fopen_s( &exeManager, "ItemManager.exe", "r" );
		if (!exeManager) {
			printf( "ItemManager.exe could not be found...\n\n" );
		} else {
			printf( "Press any key to run Jengerer's Item Manager.\n" );
			fclose( exeManager );

			_getch();

			ShellExecute( 0, 0, "ItemManager.exe", 0, 0, SW_SHOWDEFAULT );
		}
	}

	if (curl) {
		delete curl;
		curl = 0;
	}

	return EXIT_SUCCESS;
}