#pragma once

namespace SW_Server
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
			if (capacity - Position() < sizeof(val))
				throw std::overflow_error("Capacity of NetworkWriter has been met");//Grow();

			*((T*)cursor) = val;
			cursor = (void*)((size_t)cursor + sizeof(val));
		}
		template<typename T> void WriteRef(T* val)
		{
			if (capacity - Position() < sizeof(*val))
				throw std::overflow_error("Capacity of NetworkWriter has been met");//Grow();

			*((T*)cursor) = *val;
			cursor = (void*)((size_t)cursor + sizeof(val));
		}

		void Reset();
		void Grow();
		size_t Position();
		size_t Remaining();
		std::string StringCopy();
		void reset();
	};
}