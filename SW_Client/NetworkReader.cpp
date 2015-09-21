#include "NetworkReader.h"
#include <memory.h>

char* alignedBufferR = (char*)new uint32_t[1];

template<typename T>
void ClientSafeRead(char* output, char** cursor, bool swapped)
{
	memcpy(alignedBufferR, *cursor, sizeof(T));

	if (swapped)
	{
		swab(*cursor, output, sizeof(output));
	}
	else
	{
		memcpy((void*)output, (void*)*cursor, sizeof(output));
	}

	*cursor = (char*)((size_t)*cursor + sizeof(T));
}

namespace SW_Client
{
	NetworkReader::NetworkReader(char* buffer, size_t size, bool swapped) : SW::NetworkReader(buffer, size)
	{
		NetworkReader::swapped = swapped;
	}


	NetworkReader::~NetworkReader()
	{
	}

	MessageType_t NetworkReader::ReadMessageType()
	{
		if (Remaining() >= sizeof(MessageType_t))
		{
			MessageType_t output;
			ClientSafeRead<MessageType_t>((char*)&output, &cursor, swapped);
			return output;
		}
		else
		{
			return 0;
		}
	}
	uint8_t NetworkReader::ReadUint8()
	{
		if (Remaining() >= 1)
		{
			uint8_t output;
			ClientSafeRead<uint8_t>((char*)&output, &cursor, swapped);
			return output;
		}
		else
		{
			
			return 0;
		}
	}
	uint16_t NetworkReader::ReadUint16()
	{
		if (Remaining() >= 2)
		{
			uint16_t output;
			ClientSafeRead<uint16_t>((char*)&output, &cursor, swapped);
			return output;
		}
		else
		{
			
			return 0;
		}
	}
	uint32_t NetworkReader::ReadUint32()
	{
		if (Remaining() >= 4)
		{
			uint32_t output;
			ClientSafeRead<uint32_t>((char*)&output, &cursor, swapped);
			return output;
		}
		else
		{
			
			return 0;
		}
	}
	int8_t NetworkReader::ReadInt8()
	{
		if (Remaining() >= 1)
		{
			int8_t output;
			ClientSafeRead<int8_t>((char*)&output, &cursor, swapped);
			return output;
		}
		else
		{
			
			return 0;
		}
	}
	int16_t NetworkReader::ReadInt16()
	{
		if (Remaining() >= 2)
		{
			int16_t output;
			ClientSafeRead<int16_t>((char*)&output, &cursor, swapped);
			return output;
		}
		else
		{
			
			return 0;
		}
	}
	int32_t NetworkReader::ReadInt32()
	{
		if (Remaining() >= 4)
		{
			int32_t output;
			ClientSafeRead<int32_t>((char*)&output, &cursor, swapped);
			return output;
		}
		else
		{
			
			return 0;
		}
	}
	float NetworkReader::ReadSingle()
	{
		if (Remaining() >= 4)
		{
			float output;
			ClientSafeRead<float>((char*)&output, &cursor, swapped);
			return output;
		}
		else
		{
			
			return 0;
		}
	}
}
