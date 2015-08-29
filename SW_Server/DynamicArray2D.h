#pragma once

#include <glm\vec2.hpp>

namespace SW_Server
{
	template<typename T>
	class DynamicArray2D 
	{
	public:

		glm::ivec2 origin;
		glm::ivec2 dim;
		T** data;

		DynamicArray2D();
		~DynamicArray2D();

		/*void AABBNonDef(out glm::ivec2 bl, out glm::ivec2 tr);

		IEnumerable<T*> T*ryGetOutsideInside(glm::ivec2 outside, glm::ivec2 inside, int range);

		IEnumerable<T*> WithinRange(glm::ivec2 origin, int range);*/

		void GrowTopRight(glm::ivec2 amount);
		void GrowBottomLeft(glm::ivec2 amount);
		bool Contains(int x, int y);
		bool Contains(glm::ivec2 index);
		static int indexOf(glm::ivec2 index);
		static int indexOf(glm::ivec2 index, int columns);
		static int indexOf(int x, int y);
		static int indexOf(int x, int y, int columns);
		void Set(glm::ivec2 index, T* val);
		void Set(int x, int y, T* val);
		bool TryGet(int x, int y, T** val);
		T* TryGet(int x, int y);
		T* TryGet(glm::ivec2 index);
		T* Get(int x, int y);
		T* Get(glm::ivec2 index);
		T* Get2(int x, int y);

	};
}

