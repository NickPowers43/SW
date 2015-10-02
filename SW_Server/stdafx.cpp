#include "stdafx.h"
#include <SW\SW.h>
#include <SW\NetworkReader.h>
#include <SW\NetworkWriter.h>
#include "WorldQTNode.h"

namespace SW_Server
{
	WorldQTNode* qt = new WorldQTNode(0, glm::vec2(WORLD_SIZE * -0.5, WORLD_SIZE * -0.5), WORLD_SIZE, NULL);
	server* myServer;
	SW::NetworkWriter* nw_main = new SW::NetworkWriter(1 << 14);
}


using namespace SW;
using namespace SW_Server;


void SW_Server::SendPingMessage(websocketpp::connection_hdl hdl, SW::NetworkWriter* nw, size_t size)
{
	try
	{
		if (nw->Remaining() < (sizeof(MessageType_t) + sizeof(uint32_t) + size))
		{
			try
			{
				myServer->send(hdl, nw->StringCopy(), websocketpp::frame::opcode::binary);
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
			nw_main->Write((uint8_t)0);
		}
		std::string response((char*)nw->buffer, nw->Position());
		//cout << "Sending message with size: " << size;
		myServer->send(hdl, response, websocketpp::frame::opcode::binary);
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

void SW_Server::ReadPingMessageResponse(websocketpp::connection_hdl hdl, NetworkWriter* nw, NetworkReader* nr)
{
	uint32_t org_size = nr->ReadUint32();

	if ((org_size + 100) < nw->Remaining())
	{
		SendPingMessage(hdl, nw, org_size + 100);
	}
}
