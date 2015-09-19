#pragma once
#include <SW/NetworkReader.h>

namespace SW_Client
{
	class NetworkReader :
		public SW::NetworkReader
	{
	public:
		NetworkReader(char* buffer, size_t size, bool swapped);
		~NetworkReader();

		bool swapped;

		MessageType_t ReadMessageType();
		uint8_t ReadUint8();
		uint16_t ReadUint16();
		uint32_t ReadUint32();
		int8_t ReadInt8();
		int16_t ReadInt16();
		int32_t ReadInt32();
		float ReadSingle();
	};

}
