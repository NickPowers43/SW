#include "NetworkWriter.h"
#include "SW_Client.h"
#include <sstream>
#include <memory.h>

namespace SW_Client
{
	NetworkWriter::NetworkWriter(size_t capacity, bool flipped) : SW::NetworkWriter(capacity)
	{
		NetworkWriter::flipped = flipped;
	}


	NetworkWriter::~NetworkWriter()
	{
	}


	void NetworkWriter::ClientSafeWrite(char* val, size_t size)
	{
		/*if (Remaining() < size)
		{
			return;
		}*/

		/*std::ostringstream print;
		print << "writing value of size: ";
		print << size;
		PrintMessage((int)print.str().c_str());*/

		if (flipped)
		{
			swab(val, cursor, size);
		}
		else
		{
			memcpy(cursor, val, size);
		}

		cursor = (char*)((size_t)cursor + size);
	}
	void NetworkWriter::ClientSafeWrite(char* val, size_t size, char* dst)
	{
		/*if (Remaining() < size)
		{
			return;
		}*/

		if (flipped)
		{
			swab(val, dst, size);
		}
		else
		{
			memcpy(dst, val, size);
		}
	}

	void NetworkWriter::WriteMessageType(MessageType_t val)
	{
		ClientSafeWrite((char*)&val, sizeof(val));
	}
	void NetworkWriter::WriteUint8(uint8_t val)
	{
		ClientSafeWrite((char*)&val, sizeof(val));
	}
	void NetworkWriter::WriteUint16(uint16_t val)
	{
		ClientSafeWrite((char*)&val, sizeof(val));
	}
	void NetworkWriter::WriteUint32(uint32_t val)
	{
		ClientSafeWrite((char*)&val, sizeof(val));
	}
	void NetworkWriter::WriteInt8(int8_t val)
	{
		ClientSafeWrite((char*)&val, sizeof(val));
	}
	void NetworkWriter::WriteInt16(int16_t val)
	{
		ClientSafeWrite((char*)&val, sizeof(val));
	}
	void NetworkWriter::WriteInt32(int32_t val)
	{
		ClientSafeWrite((char*)&val, sizeof(val));
	}
	void NetworkWriter::WriteSingle(float val)
	{
		ClientSafeWrite((char*)&val, sizeof(val));
	}

	void NetworkWriter::WriteMessageType(MessageType_t val, char* dst)
	{
		ClientSafeWrite((char*)&val, sizeof(val), dst);
	}
	void NetworkWriter::WriteUint8(uint8_t val, char* dst)
	{
		ClientSafeWrite((char*)&val, sizeof(val), dst);
	}
	void NetworkWriter::WriteUint16(uint16_t val, char* dst)
	{
		ClientSafeWrite((char*)&val, sizeof(val), dst);
	}
	void NetworkWriter::WriteUint32(uint32_t val, char* dst)
	{
		ClientSafeWrite((char*)&val, sizeof(val), dst);
	}
	void NetworkWriter::WriteInt8(int8_t val, char* dst)
	{
		ClientSafeWrite((char*)&val, sizeof(val), dst);
	}
	void NetworkWriter::WriteInt16(int16_t val, char* dst)
	{
		ClientSafeWrite((char*)&val, sizeof(val), dst);
	}
	void NetworkWriter::WriteInt32(int32_t val, char* dst)
	{
		ClientSafeWrite((char*)&val, sizeof(val), dst);
	}
	void NetworkWriter::WriteSingle(float val, char* dst)
	{
		ClientSafeWrite((char*)&val, sizeof(val), dst);
	}
}
