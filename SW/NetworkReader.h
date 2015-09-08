#pragma once
#include "SW.h"

namespace SW
{
	class NetworkReader
	{
	public:
		NetworkReader(void* buffer, size_t size);
		~NetworkReader();

		uint8_t ReadMessageType();
		uint8_t ReadUint8();
		uint16_t ReadUint16();
		uint32_t ReadUint32();
		int8_t ReadInt8();
		int16_t ReadInt16();
		int32_t ReadInt32();
		float ReadSingle();

		void* buffer;
		void* cursor;
		size_t size;

		size_t Position();
		size_t Remaining();

		template<typename T>
		T Read(void** buffer)
		{
			T output;
			output = *((T*)*buffer);
			*buffer = (void*)((size_t)*buffer + sizeof(output));
			return output;
		}
	};
}

