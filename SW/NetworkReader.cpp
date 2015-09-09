
#include "NetworkReader.h"

//template<typename T> void Read(void** buffer, T* new_data)
//{
//	*new_data = *((T*)*buffer);
//	*buffer = (void*)((size_t)*buffer + sizeof(*new_data));
//}


namespace SW
{

	NetworkReader::NetworkReader(void* buffer, size_t size)
	{
		NetworkReader::buffer = buffer;
		NetworkReader::cursor = buffer;
		NetworkReader::size = size;
	}


	NetworkReader::~NetworkReader()
	{
	}

	MessageType_t NetworkReader::ReadMessageType()
	{
		if (Remaining() >= sizeof(MessageType_t))
		{
			return Read<MessageType_t>(&cursor);
		}
		else
		{
			printf("End of read buffer reached");
			return 0;
		}
	}
	uint8_t NetworkReader::ReadUint8()
	{
		if (Remaining() >= 1)
		{
			return Read<uint8_t>(&cursor);
		}
		else
		{
			printf("End of read buffer reached");
			return 0;
		}
	}
	uint16_t NetworkReader::ReadUint16()
	{
		if (Remaining() >= 2)
		{
			return Read<uint16_t>(&cursor);
		}
		else
		{
			printf("End of read buffer reached");
			return 0;
		}
	}
	uint32_t NetworkReader::ReadUint32()
	{
		if (Remaining() >= 4)
		{
			return Read<uint32_t>(&cursor);
		}
		else
		{
			printf("End of read buffer reached");
			return 0;
		}
	}
	int8_t NetworkReader::ReadInt8()
	{
		if (Remaining() >= 1)
		{
			return Read<uint32_t>(&cursor);
		}
		else
		{
			printf("End of read buffer reached");
			return 0;
		}
	}
	int16_t NetworkReader::ReadInt16()
	{
		if (Remaining() >= 2)
		{
			return Read<int16_t>(&cursor);
		}
		else
		{
			printf("End of read buffer reached");
			return 0;
		}
	}
	int32_t NetworkReader::ReadInt32()
	{
		if (Remaining() >= 4)
		{
			return Read<int32_t>(&cursor);
		}
		else
		{
			printf("End of read buffer reached");
			return 0;
		}
	}
	float NetworkReader::ReadSingle()
	{
		if (Remaining() >= 4)
		{
			return Read<float>(&cursor);
		}
		else
		{
			printf("End of read buffer reached");
			return 0;
		}
	}

	size_t NetworkReader::Position()
	{
		return ((size_t)cursor - (size_t)buffer);
	}
	size_t NetworkReader::Remaining()
	{
		return (size - Position());
	}
}
