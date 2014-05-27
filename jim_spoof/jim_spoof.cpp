#include "curl.hpp"

int main(int argc, char** argv)
{
	const char* executable = argv[0];

	printf("Okay, I kind of lied. There are some extra files that need to be downloaded before the new item manager runs...\n\n");
	Sleep(1500);

	Curl* curl = Curl::get_instance();
	printf("Invoking jui.dll...\n");
	std::string jui_url = "http://www.jengerer.com/item_manager/downloads/jui.dll";
	std::string jui_dest = "jui.dll";
	if (!curl->download(&jui_url, &jui_dest)) {
		printf("Invoking jui.dll failed. This likely means I'm a failure and you should download the item manager off of http://www.jengerer.com/item_manager/ instead.\n");
		Curl::shut_down();
		return -1;
	}

	std::string jutil_url = "http://www.jengerer.com/item_manager/downloads/jutil.dll";
	std::string jutil_dest = "jutil.dll";
	printf("Summoning jutil.dll...\n");
	if (!curl->download(&jutil_url, &jutil_dest)) {
		printf("The summoning has failed. This likely means I'm a failure and you should download the item manager off of http://www.jengerer.com/item_manager/ instead.\n");
		Curl::shut_down();
		return -1;
	}

	std::string png_url = "http://www.jengerer.com/item_manager/downloads/libpng15.dll";
	std::string png_dest = "libpng15.dll";
	printf("Making sure we still have libpng15.dll...\n");
	FILE* file = fopen(png_dest.c_str(), "r");
	if (file != nullptr) {
		fclose(file);
	}
	else if (!curl->download(&png_url, &png_dest)) {
		printf("Failed... This likely means I'm a failure and you should get the item manager from http://www.jengerer.com/item_manager/ instead.\n");
		Curl::shut_down();
		return -1;
	}

	std::string api_url = "http://www.jengerer.com/item_manager/downloads/steam_api.dll";
	std::string api_dest = "steam_api.dll";
	printf("Making sure we still have steam_api.dll...\n");
	file = fopen(api_dest.c_str(), "r");
	if (file != nullptr) {
		fclose(file);
	}
	else if (!curl->download(&api_url, &api_dest)) {
		printf("Failed... This likely means I'm a failure and you should get the item manager from http://www.jengerer.com/item_manager/ instead.\n");
		Curl::shut_down();
		return -1;
	}

	std::string jim_url = "http://www.jengerer.com/item_manager/downloads/item_manager.exe";
	std::string jim_dest = "item_manager.exe.new";
	printf("Finally, getting item_manager.exe...\n");
	if (!curl->download(&jim_url, &jim_dest)) {
		printf("Failed... This likely means I'm a failure and you should get the item manager from http://www.jengerer.com/item_manager/ instead.\n");
		Curl::shut_down();
		return -1;
	}

	printf("Performing rename wizardry...\n");
	DeleteFile("item_manager.exe.old");
	MoveFile(executable, "item_manager.exe.old");
	MoveFile(jim_dest.c_str(), "item_manager.exe");
	printf("Deleting auto_updater.exe. R.I.P.\n");
	DeleteFile("auto_updater.exe");
	printf("It's aliiive! Starting the new item manager...\n");
	Sleep(1500);

	// Create structs.
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	// Create copy of file name.
	if (!CreateProcess( NULL, "item_manager.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		return false;
	}

	return 0;
}