#include "stdafx.h"
#include "NetworkReader.h"

//template<typename T> void net_read(void** buffer, T* new_data)
//{
//	*new_data = *((T*)*buffer);
//	*buffer = (void*)((size_t)*buffer + sizeof(*new_data));
//}

template<typename T> T net_read(void** buffer)
{
	T output;
	output = *((T*)*buffer);
	*buffer = (void*)((size_t)*buffer + sizeof(output));
	return output;
}

namespace SW_Server
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
		if ((size - Position()) >= 1)
		{
			return net_read<MessageType_t>(&cursor);
		}
		else
		{
			throw std::exception("Read buffer overflown");
		}
	}
	uint8_t NetworkReader::ReadUint8()
	{
		if ((size - Position()) >= 1)
		{
			return net_read<uint8_t>(&cursor);
		}
		else
		{
			throw std::exception("Read buffer overflown");
		}
	}
	uint16_t NetworkReader::ReadUint16()
	{
		if ((size - Position()) >= 2)
		{
			return net_read<uint16_t>(&cursor);
		}
		else
		{
			throw std::exception("Read buffer overflown");
		}
	}
	uint32_t NetworkReader::ReadUint32()
	{
		if ((size - Position()) >= 4)
		{
			return net_read<uint32_t>(&cursor);
		}
		else
		{
			throw std::exception("Read buffer overflown");
		}
	}
	int8_t NetworkReader::ReadInt8()
	{
		if ((size - Position()) >= 1)
		{
			return net_read<uint32_t>(&cursor);
		}
		else
		{
			throw std::exception("Read buffer overflown");
		}
	}
	int16_t NetworkReader::ReadInt16()
	{
		if ((size - Position()) >= 2)
		{
			return net_read<int16_t>(&cursor);
		}
		else
		{
			throw std::exception("Read buffer overflown");
		}
	}
	int32_t NetworkReader::ReadInt32()
	{
		if ((size - Position()) >= 4)
		{
			return net_read<int32_t>(&cursor);
		}
		else
		{
			throw std::exception("Read buffer overflown");
		}
	}
	float NetworkReader::ReadSingle()
	{
		if ((size - Position()) >= 4)
		{
			return net_read<float>(&cursor);
		}
		else
		{
			throw std::exception("Read buffer overflown");
		}
	}

	size_t NetworkReader::Position()
	{
		return ((size_t)cursor - (size_t)buffer);
	}
}
