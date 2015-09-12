#pragma once
#include <SW\SW.h>
#include <vector>

namespace SW_Client
{
	class NetworkWriter;
	class Player;
	class Vessel;
}

extern SW_Client::Player* myPlayer;
extern std::vector<SW_Client::Player*> players;
extern SW_Client::Vessel* currentVessel;
extern SW_Client::NetworkWriter* nw_main;
extern bool swapBytes;

namespace SW_Client
{
	void SendMessage(NetworkWriter* nw);
	int ClearCurrentVessel();
}

extern "C"
{
	extern void SendMessage(int dPtr, int length);
	extern void CloseConnection();
	extern void PrintRefresh();
}
