#include <iostream>
#include <iomanip>
#include <fstream>
#include <conio.h>
#include <Windows.h>
#include <TlHelp32.h>

#include "Steam.h"
#include "SerializedBuffer.h"

using namespace std;

#pragma pack(push, 1)
struct TradeRequest_t {
	enum { k_iCallback = k_EMsgGCTrading_InitiateTradeRequest };
	uint16 id;
	char garbage[16];
	uint32 tradeId;
	uint64 steamId;
	//char* clientName;
};
#pragma pack(pop)

struct TradeResponse_t {
	enum { k_iCallback = k_EMsgGCTrading_InitiateTradeResponse };
	uint32 tradeResponse;
	uint32 tradeId;
};

int main()
{
	vector<uint32> tradeIds;

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
			string command;
			cout << "command: ";
			cin >> command;

			if (command == "invite") {
				int numFriends = steam.getFriends();
				bool foundName = false;
				string targetName;
				CSteamID targetId;

				cout << "Enter target name: ";
				cin >> targetName;
				for (int i=0; i<numFriends; i++) {
					CSteamID friendId = steam.getFriend( i );

					// Get information.
					FriendGameInfo_t gameInfo;
					steam.getFriendGameInfo( friendId, &gameInfo );
					if (gameInfo.m_gameID == CGameID( 440 )) {
						string friendName( steam.getPersonaName( friendId ) );
						if (targetName == friendName) {
							targetId = friendId;
							foundName = true;
							break;
						}
					}
				}

				if (foundName) {
					size_t messageSize = sizeof(TradeRequest_t);
					TradeRequest_t* sendRequest = (TradeRequest_t*)malloc(messageSize);
					memset(sendRequest, 0x00, messageSize);
					memset(sendRequest->garbage, 0xff, sizeof(sendRequest->garbage));

					sendRequest->id = 1;
					sendRequest->steamId = targetId.ConvertToUint64();
					sendRequest->tradeId = 0;

					void* voidBuffer = sendRequest;
					unsigned char* messageBuffer = (unsigned char*)voidBuffer;
					cout << "Message being sent (size " << dec << messageSize << "): ";
					for (int i=0; i<messageSize; i++) {
						cout << hex << setw(2) << setfill('0') << (unsigned int)messageBuffer[i] << " ";
					}

					cout << "\nSend message? ";
					char confirm = _getch();
					cout << "\n";

					if (confirm == 'y') {
						cout << "\nMessage sent!\n" << endl;
						steam.sendMessage( sendRequest->k_iCallback, sendRequest, messageSize );
					}
				}
			} else if (command == "accept") {
			}
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
					SerializedBuffer serializedBuffer(buffer);

					switch (id) {
					case TradeRequest_t::k_iCallback:
						{
							TradeRequest_t* tradeRequest = serializedBuffer.get<TradeRequest_t>();
							const char* clientName = (const char*)serializedBuffer.here();
							cout << "Trade request for #" << tradeRequest->tradeId << " received from " << clientName << ".\n";
						}
					case TradeResponse_t::k_iCallback:
						{
							TradeResponse_t* tradeResponse = serializedBuffer.get<TradeResponse_t>();
							cout << "Trade response " << tradeResponse->tradeResponse << " received for trade #" << tradeResponse->tradeId << ".\n";
						}

						break;
					default:
						{
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
						}

						break;
					}

					free(buffer);
				}
			}

			steam.releaseCallback();
		}
	}

	outputFile.close();
	return 0;
}