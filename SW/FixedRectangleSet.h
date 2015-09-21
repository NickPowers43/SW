#pragma once
#include "SW.h"

namespace SW
{
	template<typename T, int SizeX, int SizeY>
	class FixedRectangleSet
	{
	public:
		FixedRectangleSet()
		{
			for (size_t i = 0; i < (SizeX * SizeY); i++)
			{
				elements[i] = T();
			}
		}
		~FixedRectangleSet()
		{

		}

		void Set(glm::ivec2 index, T val)
		{
			elements[index.x + (index.y * SizeX)] = val;
		}
		T TryGet(glm::ivec2 index)
		{
			return elements[index.x + (index.y * SizeX)];
		}

		T elements[SizeX * SizeY];
	};
}

