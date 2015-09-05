#pragma once

namespace SW_Server
{
	class NetworkReader
	{
	public:
		NetworkReader(void* buffer, size_t size, bool littleEndian);
		~NetworkReader();

		uint8_t ReadMessageType();
		uint8_t ReadUint8();
		uint16_t ReadUint16();
		uint32_t ReadUint32();
		int8_t ReadInt8();
		int16_t ReadInt16();
		int32_t ReadInt32();
		float ReadSingle();

		bool littleEndian;
		void* buffer;
		void* cursor;
		size_t size;

		size_t Position();
	};
}

