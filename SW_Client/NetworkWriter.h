#pragma once
#include "../SW/NetworkWriter.h"

namespace SW_Client
{
	class NetworkWriter :
		public SW::NetworkWriter
	{
	public:

		NetworkWriter(size_t capacity, bool flipped);
		~NetworkWriter();

		bool flipped;

		void WriteMessageType(SW::MessageType_t val);
		void WriteUint8(uint8_t val);
		void WriteUint16(uint16_t val);
		void WriteUint32(uint32_t val);
		void WriteInt8(int8_t val);
		void WriteInt16(int16_t val);
		void WriteInt32(int32_t val);
		void WriteSingle(float val);

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
	};
}

