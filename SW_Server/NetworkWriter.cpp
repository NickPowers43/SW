#include "stdafx.h"
#include "NetworkWriter.h"

NetworkWriter::NetworkWriter()
{
	data = new char[MB_CAPACITY];
}
NetworkWriter::~NetworkWriter()
{
	delete data;
}
void NetworkWriter::reset()
{
	size = 0;
	temp = data;
}