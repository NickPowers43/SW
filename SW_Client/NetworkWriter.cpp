#include "NetworkWriter.h"
#include <memory.h>

char* alignedBuffer = (char*)new uint32_t[1];

template<typename T> void WriteClient(void** cursor, bool flipped, T val)
{
	*((T*)alignedBuffer) = val;

	memcpy(*cursor, alignedBuffer, sizeof(val));

	//*((T*)cursor) = val;
	*cursor = (char*)((size_t)*cursor + sizeof(val));
}

namespace SW_Client
{
	NetworkWriter::NetworkWriter(size_t capacity, bool flipped) : SW::NetworkWriter(capacity)
	{
		NetworkWriter::flipped = flipped;
	}


	NetworkWriter::~NetworkWriter()
	{
	}

	void NetworkWriter::WriteMessageType(MessageType_t val)
	{
		if (Remaining() > sizeof(val))
		{
			WriteClient<MessageType_t>(&cursor, flipped, val);
		}
		else
		{
			
		}
	}
	void NetworkWriter::WriteUint8(uint8_t val)
	{
		if (Remaining() > sizeof(val))
		{
			WriteClient<uint8_t>(&cursor, flipped, val);
		}
		else
		{
			
		}
	}
	void NetworkWriter::WriteUint16(uint16_t val)
	{
		if (Remaining() > sizeof(val))
		{
			uint16_t temp;
			swab((char*)&val, (char*)&temp, sizeof(val));

			WriteClient<uint16_t>(&cursor, flipped, temp);
		}
		else
		{
			
		}
	}
	void NetworkWriter::WriteUint32(uint32_t val)
	{
		if (Remaining() > sizeof(val))
		{
			uint32_t temp;
			swab((char*)&val, (char*)&temp, sizeof(val));

			WriteClient<uint32_t>(&cursor, flipped, temp);
		}
		else
		{
			
		}
	}
	void NetworkWriter::WriteInt8(int8_t val)
	{
		if (Remaining() > sizeof(val))
		{
			WriteClient<uint8_t>(&cursor, flipped, val);
		}
		else
		{
			
		}
	}
	void NetworkWriter::WriteInt16(int16_t val)
	{
		if (Remaining() > sizeof(val))
		{
			int16_t temp;
			swab((char*)&val, (char*)&temp, sizeof(val));

			WriteClient<int16_t>(&cursor, flipped, temp);
		}
		else
		{
			
		}
	}
	void NetworkWriter::WriteInt32(int32_t val)
	{
		if (Remaining() > sizeof(val))
		{
			int32_t temp;
			swab((char*)&val, (char*)&temp, sizeof(val));

			WriteClient<int32_t>(&cursor, flipped, temp);
		}
		else
		{
			
		}
	}
	void NetworkWriter::WriteSingle(float val)
	{
		if (Remaining() > sizeof(val))
		{
			float temp;
			swab((char*)&val, (char*)&temp, sizeof(val));

			WriteClient<float>(&cursor, flipped, temp);
		}
		else
		{
			
		}
	}
}
