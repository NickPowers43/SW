// SW_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Vessel.h"
#include "StartingVessel.h"
#include "NetworkReader.h"
#include "NetworkWriter.h"
#include "WorldQTNode.h"
#include "Player.h"
#include "StartingVessel.h"

#include <thread>
#include <queue>
#include <mutex>

NetworkWriter nw_main(1 << 14);

struct PlayerMessagePair
{
	std::string msg;
	Player* player;
};

std::mutex message_in_m;
std::queue<PlayerMessagePair> messages_in;

void SendPingMessage(websocketpp::connection_hdl hdl, NetworkWriter* nw, size_t size)
{
	try	
	{
		if (nw->Remaining() < (sizeof(MessageType_t) + sizeof(uint32_t) + size))
		{
			try 
			{
				myServer.send(hdl, nw->StringCopy(), websocketpp::frame::opcode::binary);
			}
			catch (std::overflow_error & e)
			{
				std::cout << "Failed to send ping message: " << e.what();
			}
			catch (const websocketpp::lib::error_code& e)
			{
				std::cout << "Failed to send ping message: " << e << "(" << e.message() << ")" << std::endl;
			}
		}

		if (nw->Remaining() < (sizeof(MessageType_t) + sizeof(uint32_t) + size))
			return;

		nw->Write((MessageType_t)ServerMessageType::PingMessage);
		nw->Write((uint32_t)size);
		for (size_t i = 0; i < size; i++)
		{
			nw_main.Write((uint8_t)0);
		}
		std::string response((char*)nw->buffer, nw->Position());
		//cout << "Sending message with size: " << size;
		myServer.send(hdl, response, websocketpp::frame::opcode::binary);
	}
	catch (std::overflow_error & e)
	{
		std::cout << "Failed to send ping message: " << e.what();
	}
	catch (const websocketpp::lib::error_code& e)
	{
		std::cout << "Failed to send ping message: " << e << "(" << e.message() << ")" << std::endl;
	}
}

void ReadPingMessageResponse(websocketpp::connection_hdl hdl, NetworkWriter* nw, NetworkReader* nr)
{
	nr->littleEndian = false;
	uint32_t org_size = nr->ReadUint32();
	nr->littleEndian = true;

	if (org_size < ((1 << 14) - 200))
	{
		SendPingMessage(hdl, nw, org_size + 100);
	}
}

// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {

	Player* player = players[hdl._Get()];

	if (player)
	{
		std::lock_guard<std::mutex> lock(message_in_m);

		const std::string myMsg = msg->get_payload();

		/*PlayerMessagePair pair;
		pair.msg = msg->get_payload();
		pair.player = player;
		messages_in.push(pair);*/

		NetworkReader nr((void*)myMsg.c_str(), myMsg.size(), false);
		nw_main.Reset();

		std::cout << "Message of size: " << myMsg.size() << " received.";

		while (nr.Position() < nr.size)
		{
			try	{
				MessageType_t mt = (MessageType_t)nr.ReadMessageType();
				std::string response;
				switch (mt)
				{
				case ClientMessageType::RequestModule:
					if (player->currentVessel)
					{
						player->currentVessel->ReadModuleRequestMessage(player, &nw_main, &nr);
					}
					else
					{
						throw std::exception("cannot read module request message for player not assigned a vessel");
					}
					break;
				case ClientMessageType::RequestChunk:
					if (player->currentVessel)
					{
						player->currentVessel->ReadChunkRequestMessage(player, &nw_main, &nr);
						//SendPingMessage(hdl, &nw_main, 100);
					}
					else
					{
						throw std::exception("cannot read chunk request message for player not assigned a vessel");
					}
					break;
				case ClientMessageType::PingMessageResponse:
					ReadPingMessageResponse(hdl, &nw_main, &nr);
					break;
				default:
					break;
				}
			}
			catch (std::overflow_error e) {
				std::cout << e.what();
				//Disconnect player
				break;
			}
			catch (std::exception e) {
				std::cout << e.what();
				//Disconnect player
				break;
			}
			catch (const websocketpp::lib::error_code& e) {
				std::cout << "WebSocketpp failed because: " << e << "(" << e.message() << ")" << std::endl;
				break;
			}
		}

		player->FlushBuffer(&nw_main);
	}
}

// Define a callback to handle new connections
void on_open(websocketpp::connection_hdl hdl) {
	
	Player* player = new Player(hdl);
	players[hdl._Get()] = player;
	
	StartingVessel* sV;

	if (startingVessels.size() < 1)
	{
		sV = new StartingVessel(VesselVecType(0.0f, 0.0f), 1.0f, VesselVecType(1.0f, 1.0f), 0.0f, NULL);
		startingVessels.push_back(sV);
		qt->AddVessel(sV, true);
	}
	else
	{
		sV = startingVessels[0];
	}

	sV->AddPlayer(&nw_main, player);
}

// Define a callback to handle new connections
void on_close(websocketpp::connection_hdl hdl) {
	Player* player = players[hdl._Get()];
	player->currentVessel->RemovePlayer(player);
	delete player;
	players.erase(hdl._Get());
}

bool running = true;

BOOL WINAPI OnUserClose(_In_ DWORD dwCtrlType)
{
	running = false;

	return true;
}


void game_thread_run()
{
	while (running)
	{
		QTNode* root_adj[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
		qt->UpdateSurrounding((QTNode**)&root_adj);

		int messages_in_size = 0;
		{
			/*std::lock_guard<std::mutex> lock(message_in_m);
			messages_in_size = messages_in.size();*/
		}
		while (messages_in_size > 0)
		{
			PlayerMessagePair pair;
			{
				/*std::lock_guard<std::mutex> lock(message_in_m);
				pair = messages_in.front();
				messages_in.pop();*/
			}

			
		}

		Sleep(33);
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	//string portS = string((char*)argv[1]);
	//int port = stoi(portS);
	//string ipS = string((char*)argv[1]);
	//int ip = stoi(ipS);

	Initialize();
	SetConsoleCtrlHandler(OnUserClose, true);

	std::thread game_thread(game_thread_run);

	// Create a server endpoint
	try {
		// Set logging settings
		myServer.set_access_channels(websocketpp::log::alevel::all);
		myServer.clear_access_channels(websocketpp::log::alevel::frame_payload);

		// Initialize ASIO
		myServer.init_asio();

		// Register our message handler
		myServer.set_open_handler(&on_open);
		myServer.set_message_handler(bind(&on_message, &myServer, ::_1, ::_2));
		myServer.set_close_handler(&on_close);

		myServer.set_max_message_size(1 << 16);

		// Listen on port 7778
		myServer.listen(7778);

		// Start the server accept loop
		myServer.start_accept();

		// Start the ASIO io_service run loop
		myServer.run();
	}
	catch (websocketpp::exception const & e) {
		std::cout << e.what() << std::endl;
	}
	catch (...) {
		std::cout << "other exception" << std::endl;
	}


	while (true)
	{
		
	}

	return 0;
}

