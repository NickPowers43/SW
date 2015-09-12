#include "NetworkReader.h"


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
			return Read<MessageType_t>(&cursor);
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
			return Read<uint8_t>(&cursor);
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
			uint16_t val = Read<uint16_t>(&cursor);

			if (swapped)
				swap_16((void*)&val);

			return val;
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
			uint32_t val = Read<uint32_t>(&cursor);

			if (swapped)
				swap_16((void*)&val);

			return val;
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
			return Read<uint32_t>(&cursor);
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
			int16_t val = Read<int16_t>(&cursor);

			if (swapped)
				swap_16((void*)&val);

			return val;
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
			int32_t val = Read<int32_t>(&cursor);

			if (swapped)
				swap_32((void*)&val);

			return val;
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
			float val = Read<float>(&cursor);

			if (swapped)
				swap_32((void*)&val);

			return val;
		}
		else
		{
			
			return 0;
		}
	}
}
