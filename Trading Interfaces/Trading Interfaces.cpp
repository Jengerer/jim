#include <iostream>
#include <iomanip>
#include <fstream>
#include <conio.h>
#include "Steam.h"

using namespace std;

struct TradeRequest_t {
	enum { k_iCallback = k_EMsgGCTrading_InitiateTradeRequest };
	uint16 id;
	char garbage[16];
	uint32 tradeId;
	uint64 steamId;
	char* clientName;
};

struct TradeResponse_t {
	enum { k_iCallback = k_EMsgGCTrading_InitiateTradeResponse };
	uint32 tradeResponse;
	uint32 tradeId;
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
		} else if (thisChar == 'p') {
			char* playerName = "A Noobcake's Left Nut";
			cout << strlen(playerName) << endl;
			size_t messageSize = sizeof(uint16) + sizeof(char)*16 + sizeof(uint32) + sizeof(uint64) + strlen(playerName) + 1;
			TradeRequest_t* sendRequest = (TradeRequest_t*)malloc(messageSize);
			memset(sendRequest, 0xFF, messageSize);

			void* voidBuffer = sendRequest;
			unsigned char* messageBuffer = (unsigned char*)voidBuffer;
			cout << "Message being sent: ";
			for (int i=0; i<messageSize; i++) {
				cout << hex << setw(2) << setfill('0') << (unsigned int)messageBuffer[i] << " ";
			}

			sendRequest->id = 1;
			void* thisPointer = ((unsigned char*)sendRequest) + sizeof(uint16) + sizeof(char)*16 + sizeof(uint32) + sizeof(uint64);
			memcpy(thisPointer, playerName, strlen(playerName));
			sendRequest->tradeId = 500000;
			sendRequest->steamId = steam.getSteamId();

			cout << "Message being sent: ";
			for (int i=0; i<messageSize; i++) {
				cout << hex << setw(2) << setfill('0') << (unsigned int)messageBuffer[i] << " ";
			}

			steam.sendMessage
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

					switch (id) {
					case TradeRequest_t::k_iCallback:
						{
							TradeRequest_t* tradeRequest = (TradeRequest_t*)buffer;
							cout << "Trade request #" << tradeRequest->tradeId << " received from " << tradeRequest->clientName << ".\n";
						}

						break;
					}

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