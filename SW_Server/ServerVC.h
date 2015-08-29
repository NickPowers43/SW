#pragma once
#include "VesselChunk.h"
namespace SW_Server
{

	class ServerVC : public VesselChunk
	{
	public:

		ServerVC(glm::ivec2 index, uint32_t version);
		ServerVC(glm::ivec2 index);
		~ServerVC();

	private:
		char* messageBytes;
		int messageBytesSize;
	};
}

