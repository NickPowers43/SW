#pragma once
#include <SW/NetworkWriter.h>

namespace SW_Client
{
	class NetworkWriter :
		public SW::NetworkWriter
	{
	public:

		NetworkWriter(size_t capacity, bool flipped);
		~NetworkWriter();

		bool flipped;

		void WriteMessageType(MessageType_t val);
		void WriteUint8(uint8_t val);
		void WriteUint16(uint16_t val);
		void WriteUint32(uint32_t val);
		void WriteInt8(int8_t val);
		void WriteInt16(int16_t val);
		void WriteInt32(int32_t val);
		void WriteSingle(float val);
	};
}

