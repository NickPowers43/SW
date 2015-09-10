// SW.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <thread>
#include <queue>
#include <mutex>

#include <SW\QTNode.h>
#include "WorldQTNode.h"
#include "StartingVessel.h"
#include "Vessel.h"
#include "Player.h"
#include <SW\NetworkReader.h>
#include <SW\NetworkWriter.h>

using namespace SW_Server;

std::map<void*, Player*> players;
std::vector<Vessel*> vessels;
std::vector<StartingVessel*> startingVessels;

struct PlayerMessagePair
{
	std::string msg;
	Player* player;
};

std::mutex message_in_m;
std::queue<PlayerMessagePair> messages_in;



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

		SW::NetworkReader nr((void*)myMsg.c_str(), myMsg.size());
		nw_main->Reset();

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
						player->currentVessel->ReadModuleRequestMessage(player, nw_main, &nr);
					}
					else
					{
						throw std::exception("cannot read module request message for player not assigned a vessel");
					}
					break;
				case ClientMessageType::RequestChunk:
					if (player->currentVessel)
					{
						player->currentVessel->ReadChunkRequestMessage(player, nw_main, &nr);
						//SendPingMessage(hdl, &nw_main, 100);
					}
					else
					{
						throw std::exception("cannot read chunk request message for player not assigned a vessel");
					}
					break;
				case ClientMessageType::PingMessageResponse:
					ReadPingMessageResponse(hdl, nw_main, &nr);
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

		player->FlushBuffer(nw_main);
	}
}

// Define a callback to handle new connections
void on_open(websocketpp::connection_hdl hdl) {
	
	
	StartingVessel* sV;

	if (startingVessels.size() < 1)
	{
		sV = new StartingVessel(0, VesselVecType(0.0f, 0.0f), 1.0f, VesselVecType(1.0f, 1.0f), 0.0f, NULL);
		startingVessels.push_back(sV);
		qt->AddVessel(sV, true);
	}
	else
	{
		sV = startingVessels[0];
	}

	Player* player = new Player(hdl, glm::vec2(0.0f, 0.0f), 1.0f, glm::vec2(0.0f, 0.0f), 0.0f, glm::ivec2(0, 0), NULL);
	players[hdl._Get()] = player;
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
		SW::QTNode* root_adj[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
		qt->UpdateSurrounding((SW::QTNode**)&root_adj);

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

	SW::Initialize();
	SetConsoleCtrlHandler(OnUserClose, true);

	std::thread game_thread(game_thread_run);

	myServer = new server();

	// Create a server endpoint
	try {
		// Set logging settings
		myServer->set_access_channels(websocketpp::log::alevel::all);
		myServer->clear_access_channels(websocketpp::log::alevel::frame_payload);

		// Initialize ASIO
		myServer->init_asio();

		// Register our message handler
		myServer->set_open_handler(&on_open);
		myServer->set_message_handler(bind(&on_message, myServer, ::_1, ::_2));
		myServer->set_close_handler(&on_close);

		myServer->set_max_message_size(1 << 16);

		// Listen on port 7778
		myServer->listen(7778);

		// Start the server accept loop
		myServer->start_accept();

		// Start the ASIO io_service run loop
		myServer->run();
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

