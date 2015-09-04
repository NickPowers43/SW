// SW_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Vessel.h"
#include "StartingVessel.h"
#include "NetworkReader.h"


// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {

	Player* player = players[hdl._Get()];

	if (player)
	{
		NetworkReader nr((void*)msg->get_raw_payload().c_str(), msg->get_raw_payload().size());

		while (nr.Position() < nr.size)
		{
			ClientMessageType::ClientMessageType mt = (ClientMessageType::ClientMessageType)nr.ReadMessageType();

			switch (mt)
			{
			case ClientMessageType::RequestChunk:
				if (player->currentVessel)
					player->currentVessel->ReadChunkRequestMessage(player, &nr);
				//else
					//throw std::exception("cannot read chunk request message for player not assigned a vessel");
				break;
			default:
				break;
			}

			try	{
				
			}
			catch (std::exception e) {
				cout << e.what();
				//Disconnect player
				break;
			}
		}
	}
	//std::cout << "on_message called with hdl: " << hdl.lock().get()
	//	<< " and message: " << msg->get_payload()
	//	<< std::endl;

	//// check for a special command to instruct the server to stop listening so
	//// it can be cleanly exited.
	//if (msg->get_payload() == "stop-listening") {
	//	s->stop_listening();
	//	return;
	//}

	//try {
	//	s->send(hdl, msg->get_payload(), msg->get_opcode());
	//}
	//catch (const websocketpp::lib::error_code& e) {
	//	std::cout << "Echo failed because: " << e
	//		<< "(" << e.message() << ")" << std::endl;
	//}
}

// Define a callback to handle new connections
void on_open(websocketpp::connection_hdl hdl) {
	
	Player* player = new Player(hdl);
	players[hdl._Get()] = player;
	
	if (startingVessels.size() < 1)
	{
		startingVessels.push_back(new StartingVessel());
		startingVessels[0]->Initialize();
	}

	startingVessels[0]->AddPlayer(player);
}

// Define a callback to handle new connections
void on_close(websocketpp::connection_hdl hdl) {
	Player* player = players[hdl._Get()];
	player->currentVessel->RemovePlayer(player);
	delete player;
	players.erase(hdl._Get());
}



int _tmain(int argc, _TCHAR* argv[])
{
	//string portS = string((char*)argv[1]);
	//int port = stoi(portS);
	//string ipS = string((char*)argv[1]);
	//int ip = stoi(ipS);

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

