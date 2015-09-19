#pragma once
#include "SW.h"

namespace SW
{
	class NetworkReader
	{
	public:
		NetworkReader(char* buffer, size_t size);
		~NetworkReader();

		uint8_t ReadMessageType();
		uint8_t ReadUint8();
		uint16_t ReadUint16();
		uint32_t ReadUint32();
		int8_t ReadInt8();
		int16_t ReadInt16();
		int32_t ReadInt32();
		float ReadSingle();

		char* buffer;
		char* cursor;
		size_t size;

		size_t Position();
		size_t Remaining();
	};
}

