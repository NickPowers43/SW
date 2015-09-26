#include "SW.h"
#include "VesselModuleTemplate.h"
#include <glm/gtc/matrix_transform.hpp>

float deltaTime = 0.0f;
float elapsedTime = 0.0f;

namespace SW
{
	VesselModuleTemplate* ConstructSimpleFrigateCore()
	{
		glm::ivec2 dim(3, 8);
		VesselModuleTemplate* output = new VesselModuleTemplate(dim);
		output->tiles.BuildFoundation(glm::ivec2(0, 0), dim + glm::ivec2(1,1));

		glm::ivec2 temp = glm::ivec2(0, 0);
		output->tiles.BuildWall(&temp, 8, WallType::ZeroByOne, false);
		output->tiles.BuildWall(&temp, 3, WallType::OneByZero, false);
		output->tiles.BuildWall(&temp, 8, WallType::ZeroByOne, true);
		output->tiles.BuildWall(&temp, 3, WallType::OneByZero, true);

		output->tiles.RebuildCompartments(true);
		output->tiles.SetCompartmentFloorAt(FloorType::Basic, glm::vec2(1.5f, 1.5f));

		return output;
	}
	VesselModuleTemplate* ConstructSimpleFrigateBridge()
	{
		glm::ivec2 dim(3, 9);
		VesselModuleTemplate* output = new VesselModuleTemplate(dim);
		output->tiles.BuildFoundation(glm::ivec2(0, 0), dim + glm::ivec2(1, 1));

		glm::ivec2 temp = glm::ivec2(0, 0);
		output->tiles.BuildWall(&temp, 7, WallType::ZeroByOne, false);
		output->tiles.BuildWall(&temp, 1, WallType::OneByTwo, false);
		output->tiles.BuildWall(&temp, 1, WallType::OneByZero, false);
		output->tiles.BuildWall(&temp, 1, WallType::OneByTwoFlipped, true);
		output->tiles.BuildWall(&temp, 7, WallType::ZeroByOne, true);
		output->tiles.BuildWall(&temp, 3, WallType::OneByZero, true);

		output->tiles.RebuildCompartments(true);
		output->tiles.SetCompartmentFloorAt(FloorType::Basic, glm::vec2(1.5f, 1.5f));

		return output;
	}

	VesselModuleTemplate** vesselModuleTemplates;


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

	float* wallMagnitudes = new float[9]{
		0.0f,
		1.0f,
		5.0f,
		2.0f,
		5.0f,
		1.0f,
		5.0f,
		2.0f,
		5.0f
	};

	float* wallRotations = new float[9]{
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f
	};


	void Initialize()
	{
		for (size_t i = 1; i < 9; i++)
		{
			wallVectorsNormalized[i] = glm::normalize(wallVectorsNormalized[i]);
		}
		for (size_t i = 1; i < 9; i++)
		{
			wallMagnitudes[i] = glm::sqrt(wallMagnitudes[i]);
		}
		for (size_t i = 1; i < 9; i++)
		{
			wallRotations[i] = glm::acos(wallVectorsNormalized[i].x);
		}

		vesselModuleTemplates = new VesselModuleTemplate*[2];
		vesselModuleTemplates[VMType::SimpleFrigateCore] = ConstructSimpleFrigateCore();
		vesselModuleTemplates[VMType::SimpleFrigateBridge] = ConstructSimpleFrigateBridge();
	}
}

void swap_16(void* val)
{
	char temp = *(char*)val;
	char* temp0 = (char*)((size_t)val + 1);
	*(char*)val = *temp0;
	*temp0 = temp;
}

void swap_32(void* val)
{
	char temp = *(char*)val;
	char* temp0 = (char*)((size_t)val + 3);
	*(char*)val = *temp0;
	*temp0 = temp;

	temp = *(char*)((size_t)val + 1);
	temp0 = (char*)((size_t)val + 2);
	*(char*)((size_t)val + 1) = *temp0;
	*temp0 = temp;
}