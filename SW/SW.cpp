#include "SW.h"
#include "VesselModuleTemplate.h"

namespace SW
{
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

	void Initialize()
	{
		for (size_t i = 0; i < 9; i++)
		{
			wallVectorsNormalized[i] = glm::normalize(wallVectorsNormalized[i]);
		}

		vesselModuleTemplates = new VesselModuleTemplate*[2];
		vesselModuleTemplates[0] = new VesselModuleTemplate(glm::ivec2(1, 1));
		vesselModuleTemplates[1] = new VesselModuleTemplate(glm::ivec2(1, 1));
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