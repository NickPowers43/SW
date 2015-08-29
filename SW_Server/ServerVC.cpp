#include "stdafx.h"
#include "ServerVC.h"


namespace SW_Server
{
	ServerVC::ServerVC(glm::ivec2 index, uint32_t version) : VesselChunk(index)
	{
		ServerVC::version = version;
	}
	ServerVC::ServerVC(glm::ivec2 index) : VesselChunk(index)
	{

	}


	ServerVC::~ServerVC()
	{
	}

}