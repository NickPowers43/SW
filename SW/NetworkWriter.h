#pragma once
#include "SW.h"
#include <string>

namespace SW
{
	class NetworkWriter
	{
	public:
		NetworkWriter(size_t capacity);
		~NetworkWriter();

		size_t capacity;
		void* buffer;
		void* cursor;

		template<typename T> void Write(T val)
		{
			*((T*)cursor) = val;
			cursor = (void*)((size_t)cursor + sizeof(val));
		}

		void Reset();
		//void Grow();
		size_t Position();
		size_t Remaining();
		std::string StringCopy();
		void reset();
	};
}