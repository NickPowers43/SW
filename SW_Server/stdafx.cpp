#include "stdafx.h"

glm::ivec2* wallOffsets = new glm::ivec2[9] {
	glm::ivec2(0, 0),
	glm::ivec2(1, 0),
	glm::ivec2(2, 1),
	glm::ivec2(1, 1),
	glm::ivec2(1, 2),
	glm::ivec2(0, 1),
	glm::ivec2(-1, 2),
	glm::ivec2(-1, 1),
	glm::ivec2(-2, 1)
};

glm::vec2* wallVectorsNormalized = new glm::vec2[9] {
	glm::vec2(0.0f, 0.0f),
	glm::vec2(1.0f, 0.0f),
	glm::vec2(2.0f, 1.0f),
	glm::vec2(1.0f, 1.0f),
	glm::vec2(1.0f, 2.0f),
	glm::vec2(0.0f, 1.0f),
	glm::vec2(-1.0f, 2.0f),
	glm::vec2(-1.0f, 1.0f),
	glm::vec2(-2.0f, 1.0f)
};

namespace SW_Server
{
	void Initialize()
	{
		for (size_t i = 0; i < 9; i++)
		{
			wallVectorsNormalized[i] = glm::normalize(wallVectorsNormalized[i]);
		}
	}
}

server myServer;
std::map<void*, Player*> players;
std::vector<Vessel*> vessels;
std::vector<StartingVessel*> startingVessels;