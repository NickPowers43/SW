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

void swap_16(void* val)
{
	char temp = *(char*)val;
	char* temp0 = (char*)((size_t)val + 1);
	*(char*)val = *temp0;
	*temp0 = temp;
}

void swap_32(void* val)
{
	char temp = *(char*)val;
	char* temp0 = (char*)((size_t)val + 3);
	*(char*)val = *temp0;
	*temp0 = temp;

	temp = *(char*)((size_t)val + 1);
	temp0 = (char*)((size_t)val + 2);
	*(char*)((size_t)val + 1) = *temp0;
	*temp0 = temp;
}

namespace SW_Server
{

	NetworkReader::NetworkReader(void* buffer, size_t size, bool littleEndian)
	{
		NetworkReader::buffer = buffer;
		NetworkReader::cursor = buffer;
		NetworkReader::size = size;
		NetworkReader::littleEndian = littleEndian;
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
			throw std::exception("End of read buffer reached");
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
			throw std::exception("End of read buffer reached");
		}
	}
	uint16_t NetworkReader::ReadUint16()
	{
		if ((size - Position()) >= 2)
		{
			uint16_t val = net_read<uint16_t>(&cursor);

			if (littleEndian)
				swap_16((void*)&val);

			return val;
		}
		else
		{
			throw std::exception("End of read buffer reached");
		}
	}
	uint32_t NetworkReader::ReadUint32()
	{
		if ((size - Position()) >= 4)
		{
			uint32_t val = net_read<uint32_t>(&cursor);

			if (littleEndian)
				swap_16((void*)&val);

			return val;
		}
		else
		{
			throw std::exception("End of read buffer reached");
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
			throw std::exception("End of read buffer reached");
		}
	}
	int16_t NetworkReader::ReadInt16()
	{
		if ((size - Position()) >= 2)
		{
			int16_t val = net_read<int16_t>(&cursor);

			if (littleEndian)
				swap_16((void*)&val);

			return val;
		}
		else
		{
			throw std::exception("End of read buffer reached");
		}
	}
	int32_t NetworkReader::ReadInt32()
	{
		if ((size - Position()) >= 4)
		{
			int32_t val = net_read<int32_t>(&cursor);

			if (littleEndian)
				swap_32((void*)&val);

			return val;
		}
		else
		{
			throw std::exception("End of read buffer reached");
		}
	}
	float NetworkReader::ReadSingle()
	{
		if ((size - Position()) >= 4)
		{
			float val = net_read<float>(&cursor);

			if (littleEndian)
				swap_32((void*)&val);

			return val;
		}
		else
		{
			throw std::exception("End of read buffer reached");
		}
	}

	size_t NetworkReader::Position()
	{
		return ((size_t)cursor - (size_t)buffer);
	}
}
