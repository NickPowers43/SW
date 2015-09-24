#include "SW.h"
#include "Tile.h"
#include "TileSet.h"

using namespace SW;

float Determinant(glm::vec2 c0, glm::vec2 c1)
{
	return (c0.x * c1.y) - (c0.y * c1.x);
}
glm::vec2 WallCorner(glm::vec2 wall0, glm::vec2 wall1)
{
	glm::vec2 d(-wall0.y, wall0.x);

	float uDenom = wall1.x - wall0.x;
	float uNum;

	if (abs(uDenom) > 0.001f)
	{
		uNum = d.x - wall1.y;
	}
	else
	{
		//may be straight
		uDenom = wall1.y - wall0.y;
		uNum = d.y + wall1.x;
	}

	return d + ((wall0)* (uNum / uDenom));
}
WallType_t CCWReverseWallSweepOpposite(int start, int stop, TileSet* ts, glm::ivec2 location, WallType_t type)
{
	for (int i = start; i < stop; i++)
	{
		if (abs(i - type) <= 4)
		{
			SW::Tile* tile;
			if ((tile = ts->TryGet(location - SW::wallOffsets[i])) && tile->Contains(i))
			{
				return i;
			}
		}
	}
	return 0;
}
WallType_t CCWReverseWallSweepLocal(int start, int stop, TileSet* ts, glm::ivec2 location, WallType_t type)
{
	for (int i = start; i < stop; i++)
	{
		if (abs(i - type) > 3)
		{
			SW::Tile* tile;
			if ((tile = ts->TryGet(location - SW::wallOffsets[i])) && tile->Contains(i))
			{
				return i;
			}
		}
	}
	return 0;
}
WallType_t CCWWallSweepOpposite(int start, int stop, Tile* orgtile, WallType_t type)
{
	for (int i = start; i < stop; i++)
	{
		if (abs(i - type) <= 4)
		{
			if (orgtile->Contains(i))
			{
				return i;
			}
		}
	}
	return 0;
}
WallType_t CCWWallSweepLocal(int start, int stop, Tile* orgtile, WallType_t type)
{
	for (int i = start; i < stop; i++)
	{
		if (abs(i - type) > 3)
		{
			if (orgtile->Contains(i))
			{
				return i;
			}
		}
	}
	return 0;
}

glm::vec2 Rotate90CCW(glm::vec2 v)
{
	return glm::vec2(-v.y, v.x);
}
glm::vec2 Rotate90CW(glm::vec2 v)
{
	return glm::vec2(v.y, -v.x);
}
bool WallVertexSweepCCW(SW::TileSet* ts, SW::Tile* orgtile, glm::ivec2 location, WallType_t type, bool end, glm::vec2 & v)
{
	WallType_t otherWall = 0;

	if (!end)
	{
		if ((otherWall = CCWWallSweepLocal(type, 9, orgtile, type)))
		{
			v = WallCorner(SW::wallVectorsNormalized[type], SW::wallVectorsNormalized[otherWall]);
			return false;
		}
		if ((otherWall = CCWReverseWallSweepOpposite(1, 9, ts, location, type)))
		{
			v = WallCorner(SW::wallVectorsNormalized[type], -SW::wallVectorsNormalized[otherWall]);
			return false;
		}
		if ((otherWall = CCWWallSweepLocal(1, type, orgtile, type)))
		{
			v = WallCorner(SW::wallVectorsNormalized[type], SW::wallVectorsNormalized[otherWall]);
			return false;
		}
		glm::vec2 temp = SW::wallVectorsNormalized[type];
		v = Rotate90CCW(temp) - temp;
		return true;
	}
	else
	{
		if ((otherWall = CCWReverseWallSweepLocal(type, 9, ts, location, type)))
		{
			v = WallCorner(-SW::wallVectorsNormalized[type], -SW::wallVectorsNormalized[otherWall]);
			return false;
		}
		if ((otherWall = CCWWallSweepOpposite(1, 9, orgtile, type)))
		{
			v = WallCorner(-SW::wallVectorsNormalized[type], SW::wallVectorsNormalized[otherWall]);
			return false;
		}
		if ((otherWall = CCWReverseWallSweepLocal(1, type, ts, location, type)))
		{
			v = WallCorner(-SW::wallVectorsNormalized[type], -SW::wallVectorsNormalized[otherWall]);
			return false;
		}
		glm::vec2 temp = -SW::wallVectorsNormalized[type];
		v = Rotate90CCW(temp) - temp;
		return true;
	}
}
bool WallVertexSweepCW(SW::TileSet* ts, SW::Tile* orgtile, glm::ivec2 location, WallType_t type, bool end, glm::vec2 & v)
{
	SW::Tile* tile;

	if (!end)
	{
		for (int i = type; i > 0; i--)
		{
			if (abs(i - type) > 3)
			{
				if (orgtile->Contains(i))
				{
					//calculate the corner vertice location
					v = WallCorner(SW::wallVectorsNormalized[i], SW::wallVectorsNormalized[type]);
					return false;
				}
			}
		}
		for (int i = 8; i > 0; i--)
		{
			if (abs(i - type) <= 4)
			{
				if ((tile = ts->TryGet(location - SW::wallOffsets[i])) && tile->Contains(i))
				{
					//calculate the corner vertice location
					v = WallCorner(-SW::wallVectorsNormalized[i], SW::wallVectorsNormalized[type]);
					return false;
				}
			}
		}
		for (int i = 8; i > type; i--)
		{
			if (abs(i - type) > 3)
			{
				if (orgtile->Contains(i))
				{
					//calculate the corner vertice location
					v = WallCorner(SW::wallVectorsNormalized[i], SW::wallVectorsNormalized[type]);
					return false;
				}
			}
		}
		glm::vec2 temp = SW::wallVectorsNormalized[type];
		v = Rotate90CW(temp) - temp;
		return true;
	}
	else
	{
		for (int i = type; i > 0; i--)
		{
			if (abs(i - type) > 3)
			{
				if ((tile = ts->TryGet(location - SW::wallOffsets[i])) && tile->Contains(i))
				{
					//calculate the corner vertice location
					v = WallCorner(-SW::wallVectorsNormalized[i], -SW::wallVectorsNormalized[type]);
					return false;
				}
			}
		}
		for (int i = 8; i > 0; i--)
		{
			if (abs(i - type) <= 4)
			{
				if (orgtile->Contains(i))
				{
					//calculate the corner vertice location
					v = WallCorner(SW::wallVectorsNormalized[i], -SW::wallVectorsNormalized[type]);
					return false;
				}
			}
		}
		for (int i = 8; i > type; i--)
		{
			if (abs(i - type) > 3)
			{
				if ((tile = ts->TryGet(location - SW::wallOffsets[i])) && tile->Contains(i))
				{
					//calculate the corner vertice location
					v = WallCorner(-SW::wallVectorsNormalized[i], -SW::wallVectorsNormalized[type]);
					return false;
				}
			}
		}
		glm::vec2 temp = -SW::wallVectorsNormalized[type];
		v = Rotate90CW(temp) - temp;
		return true;
	}
}
