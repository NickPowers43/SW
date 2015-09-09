#include "NetworkWriter.h"

namespace SW_Client
{
	NetworkWriter::NetworkWriter(size_t capacity, bool flipped) : SW::NetworkWriter(capacity)
	{
		NetworkWriter::flipped = flipped;
	}


	NetworkWriter::~NetworkWriter()
	{
	}

	void NetworkWriter::WriteMessageType(SW::MessageType_t val)
	{
		if (Remaining() < sizeof(val))
		{
			Write(val);
		}
		else
		{
			printf("End of read buffer reached");
		}
	}
	void NetworkWriter::WriteUint8(uint8_t val)
	{
		if (Remaining() < sizeof(val))
		{
			Write(val);
		}
		else
		{
			printf("End of read buffer reached");
		}
	}
	void NetworkWriter::WriteUint16(uint16_t val)
	{
		if (Remaining() < sizeof(val))
		{
			if (flipped)
				swap_16((void*)&val);

			Write(val);
		}
		else
		{
			printf("End of read buffer reached");
		}
	}
	void NetworkWriter::WriteUint32(uint32_t val)
	{
		if (Remaining() < sizeof(val))
		{
			if (flipped)
				swap_32((void*)&val);

			Write(val);
		}
		else
		{
			printf("End of read buffer reached");
		}
	}
	void NetworkWriter::WriteInt8(int8_t val)
	{
		if (Remaining() < sizeof(val))
		{
			Write(val);
		}
		else
		{
			printf("End of read buffer reached");
		}
	}
	void NetworkWriter::WriteInt16(int16_t val)
	{
		if (Remaining() < sizeof(val))
		{
			if (flipped)
				swap_16((void*)&val);

			Write(val);
		}
		else
		{
			printf("End of read buffer reached");
		}
	}
	void NetworkWriter::WriteInt32(int32_t val)
	{
		if (Remaining() < sizeof(val))
		{
			if (flipped)
				swap_32((void*)&val);

			Write(val);
		}
		else
		{
			printf("End of read buffer reached");
		}
	}
	void NetworkWriter::WriteSingle(float val)
	{
		if (Remaining() < sizeof(val))
		{
			if (flipped)
				swap_32((void*)&val);

			Write(val);
		}
		else
		{
			printf("End of read buffer reached");
		}
	}
}