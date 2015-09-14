#include "NetworkReader.h"
#include <memory.h>

char* alignedBufferR = (char*)new uint32_t[1];

template<typename T>
T ReadClient(void** cursor, bool swapped)
{
	memcpy(alignedBufferR, *cursor, sizeof(T));

	T val = *((T*)alignedBufferR);
	*cursor = (void*)((size_t)*cursor + sizeof(val));

	if (sizeof(val) > 1)
	{
		if (!swapped)
		{
			return val;
		}

		T temp;
		swab((char*)&val, (char*)&temp, sizeof(val));
		return temp;
	}
	else
	{

		return val;
	}
}

namespace SW_Client
{
	NetworkReader::NetworkReader(void* buffer, size_t size, bool swapped) : SW::NetworkReader(buffer, size)
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
			return ReadClient<MessageType_t>(&cursor, swapped);
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
			return ReadClient<uint8_t>(&cursor, swapped);
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
			return ReadClient<uint16_t>(&cursor, swapped);
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
			return ReadClient<uint32_t>(&cursor, swapped);
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
			return ReadClient<int8_t>(&cursor, swapped);
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
			return ReadClient<int16_t>(&cursor, swapped);
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
			return ReadClient<int32_t>(&cursor, swapped);
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
			return ReadClient<float>(&cursor, swapped);
		}
		else
		{
			
			return 0;
		}
	}
}
