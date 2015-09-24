#pragma once
#include <glm/vec2.hpp>

namespace SW_Client
{
	struct SpriteRect
	{
		float x;
		float y;
		float width;
		float height;

		SpriteRect();
		SpriteRect(glm::vec2 bl, glm::vec2 dim);
	};
}

