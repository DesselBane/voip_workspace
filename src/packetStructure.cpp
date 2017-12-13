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


void PacketStrcutre::WriteDataToBuffer ( const uint32_t data, int const startBit, int const endBit )
{
	ValidateDataLength(data, endBit - startBit + 1);
	
	int tail = 8 - endBit % 8 - 1;
	int head = startBit % 8;
	int position = (endBit + tail) / 8;

	int headBitMask = (1 << head) - 1 << 8 - head;
	int tailBitMask = (1 << tail) - 1;
	int bitMask = headBitMask | tailBitMask;

	char oldData = bitMask & byteBuffer_[position];
	
	const int last8Bitmask = 1 << 8 - 1;

	byteBuffer_[position] = data << tail & last8Bitmask | oldData; //TODO verify this

	int forntHead = 8 - tail;
	int remainingBits = endBit - startBit - forntHead;
	int remainingData = data >> forntHead;
	position--;


	while ( remainingBits >= 8 && position >= 0)
	{
		byteBuffer_[position] = remainingData & last8Bitmask;

		position--;
		remainingData >>= 8;
		remainingBits -= 8;
	}

	if(remainingBits < 1)
		return;

	oldData = byteBuffer_[position] & (1 << 8 - remainingBits) - 1 << remainingBits;
	byteBuffer_[position] = oldData | remainingData & last8Bitmask;
}

uint32_t PacketStrcutre::ReadDataFromBuffer ( int const startBit, int const endBit )
{
	int totalLength = (endBit - startBit) + 1;

	if (totalLength > 32)
		throw "Cannot read more then a unit32_t";

	int tail = 8 - endBit % 8 - 1;
	int position = (endBit + tail) / 8;
	int head = startBit % 8;
	int length = 8 - head < totalLength ? 8 - head : totalLength;

	int bitMask = (1 << length) - 1 << tail;
	int headData = (byteBuffer_[position] & bitMask) >> tail;

	int remainingBits = totalLength - length;
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


