#include "packetStructure.h"


void PacketStrcutre::ValidateDataLength ( int const data, int const length )
{
	if (data - ((1 << length) - 1) > 0)
		throw "Data exceeds length";
}

void PacketStrcutre::CreateByteBuffer ( int const sizeByte )
{
	if (byteBuffer_ != nullptr)
	{
		byteBuffer_ = new char[sizeByte];
		sizeByte_ = sizeByte;
	}
}


void PacketStrcutre::WriteDataToBuffer ( int const data, int const startBit, int const endBit )
{
	ValidateDataLength(data, endBit - startBit + 1);

	int tail = 8 - endBit % 8 - 1;
	int position = (endBit + tail) / 8;
	char oldData = ((1 << tail) - 1) & byteBuffer_[position];

}


PacketStrcutre::~PacketStrcutre ( )
{
	delete[] byteBuffer_;
	byteBuffer_ = nullptr;
}
char* PacketStrcutre::getBuffer ( )
{
	return byteBuffer_;
}
int PacketStrcutre::getSizeByte ( )
{
	return sizeByte_;
}


