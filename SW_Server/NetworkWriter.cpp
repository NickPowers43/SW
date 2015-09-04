#include "stdafx.h"
#include "NetworkWriter.h"

namespace SW_Server
{
	NetworkWriter::NetworkWriter(size_t capacity)
	{
		NetworkWriter::capacity = capacity;
		NetworkWriter::buffer = new char[capacity];
		NetworkWriter::cursor = buffer;
	}
	NetworkWriter::~NetworkWriter()
	{
		delete buffer;
	}

	void NetworkWriter::Grow()
	{
		void* temp = new char[capacity + (capacity >> 1)];
		memcpy(temp, buffer, capacity);
		capacity += capacity >> 1;
	}
	void NetworkWriter::Reset()
	{
		cursor = buffer;
	}
	size_t NetworkWriter::Position()
	{
		return ((size_t)cursor - (size_t)buffer);
	}
}