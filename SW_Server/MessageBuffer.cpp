#include "stdafx.h"
#include "MessageBuffer.h"

MessageBuffer::MessageBuffer()
{
	data = new char[MB_CAPACITY];
}
MessageBuffer::~MessageBuffer()
{
	delete data;
}
void MessageBuffer::reset()
{
	size = 0;
	temp = data;
}