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
		char* buffer;
		char* cursor;

		template<typename T> void Write(T val)
		{
			//if (capacity - Position() < sizeof(val))
			//	cerr << "Capacity of NetworkWriter has been met";

			*((T*)cursor) = val;
			cursor = (char*)((size_t)cursor + sizeof(val));
		}
		template<typename T> void WriteRef(T* val)
		{
			//if (capacity - Position() < sizeof(*val))
			//	cerr << "Capacity of NetworkWriter has been met";

			*((T*)cursor) = *val;
			cursor = (char*)((size_t)cursor + sizeof(val));
		}

		void Reset();
		//void Grow();
		size_t Position();
		size_t Remaining();
		std::string StringCopy();
		void reset();
	};
}