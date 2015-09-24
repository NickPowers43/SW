#include "SpriteRect.h"


namespace SW_Client
{
	SpriteRect::SpriteRect()
	{
		x = 0.0f;
		y = 0.0f;
		width = 0.0f;
		height = 0.0f;
	}
	SpriteRect::SpriteRect(glm::vec2 bl, glm::vec2 dim)
	{
		x = bl.x;
		y = bl.y;
		width = dim.x;
		height = dim.y;
	}
}
