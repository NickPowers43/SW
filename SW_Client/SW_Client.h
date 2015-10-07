#pragma once
#include <SW\SW.h>
#include <vector>
#include <map>

#define FLOOR_DEPTH 0.0f;
#define WALL_DEPTH 0.1f;

namespace SW_Client
{
	class NetworkReader;
	class NetworkWriter;
	class Player;
	class Vessel;
}

extern uint8_t* keyStates;
extern SW_Client::Player* myPlayer;
extern SW_Client::Vessel* currentVessel;
extern SW::Compartment* playerComp;
extern std::map<VesselID_t, SW_Client::Vessel*> activeVessels;
extern std::map<VesselID_t, SW_Client::Vessel*> vessels;
extern std::map<PlayerID_t, SW_Client::Player*> players;
extern SW_Client::NetworkWriter* nw_main;
extern bool swapBytes;

namespace SW_Client
{
	void FlushBuffer(NetworkWriter* nw);
	void StepCurrentVessel();
	void DrawWorld();
	void DrawTiles(SW::TileSet* ts, SW::AABBi region);
	void GenerateFloorMesh(SW::TileSet* ts, SW::AABBi region);
}


extern "C"
{
	extern void PrintMessage(int cStr);
	extern void SendMessage(int dPtr, int length);
	extern void DisplayMainMenu();
	extern void HideMainMenu();
	extern void CloseConnection();
	extern void PrintRefresh();
	extern void StartProgram();
}
