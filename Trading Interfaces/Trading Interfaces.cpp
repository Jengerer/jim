#include <iostream>
#include <iomanip>
#include <fstream>
#include <conio.h>
#include "Steam.h"

using namespace std;

struct TradeRequest_t {
	uint16 id;
	char garbage[16];
	uint64 steamId;
};

int main()
{
	Steam steam;
	try {
		steam.openInterfaces();
	}
	catch (Exception &) {
		cout << "Failed to initialize Steam interfaces.\n";
		return EXIT_FAILURE;
	}

	ofstream outputFile;
	outputFile.open("output.txt");

	CallbackMsg_t callback;
	int lastCallback = 0;
	bool shouldEnd = false;
	while (!shouldEnd) {
		char thisChar = _getch();
		if (thisChar == ' ') {
			break;
		}

		if (steam.getCallback(&callback)) {
			if (lastCallback != callback.m_iCallback) {
				lastCallback = callback.m_iCallback;
				outputFile << "Got callback of type " << callback.m_iCallback << " == " << GCMessageAvailable_t::k_iCallback << ".\n";
				cout << "Got callback of type " << callback.m_iCallback << " == " << GCMessageAvailable_t::k_iCallback << ".\n";
			}

			if (callback.m_iCallback == GCMessageAvailable_t::k_iCallback) {
				// Check for a message.
				uint32 size;
				if (steam.hasMessage(&size)) {
					// Get the message.
					unsigned int id, real;
					void* buffer = malloc(size);
					steam.getMessage(&id, buffer, size, &real);

					// Output message types.
					outputFile << "Message received of type " << id << "!\n";
					cout << "Message received of type " << id << "!\n";
					unsigned char* bufferArray = (unsigned char*)buffer;
					for (unsigned int i=0; i<size; i++) {
						cout << hex << setw(2) << setfill('0') << (unsigned int)bufferArray[i] << " ";
						outputFile << hex << setw(2) << setfill('0') << (unsigned int)bufferArray[i] << " ";
					}
					outputFile << "\n\n";
					cout << "\n\n";

					free(buffer);
				}
			}

			steam.releaseCallback();
		}
	}

	outputFile.close();
	return 0;
}