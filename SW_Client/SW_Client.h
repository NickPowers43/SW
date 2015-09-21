#pragma once
#include <SW\SW.h>
#include <vector>

#define FLOOR_DEPTH 0.0f;
#define WALL_DEPTH 0.1f;

namespace SW_Client
{
	class NetworkReader;
	class NetworkWriter;
	class Player;
	class Vessel;
}

extern SW_Client::Player* myPlayer;
extern SW_Client::Vessel* currentVessel;
extern SW_Client::NetworkWriter* nw_main;
extern bool swapBytes;

namespace SW_Client
{
	void FlushBuffer(NetworkWriter* nw);
}


extern "C"
{

	extern void PrintMessage(int cStr);
	extern void SendMessage(int dPtr, int length);
	extern void CloseConnection();
	extern void PrintRefresh();
	extern void StartProgram();
}
