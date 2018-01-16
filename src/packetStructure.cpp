#include "PacketStructure.h"


void PacketStructure::ValidateDataLength(int const data, int const length)
{
	long longData = data;
	if (longData >> (length - 1) > 1)
		throw "Data exceeds length";
}

void PacketStructure::CreateByteBuffer(int const sizeByte)
{
	if (byteBuffer_ == nullptr)
	{
		byteBuffer_ = new std::vector<uint8_t>(sizeByte);
		sizeByte_ = sizeByte;
	}

	for (int i = 0; i < sizeByte_; i++)
		byteBuffer_->data()[i] = 0;
}


void PacketStructure::WriteDataToBuffer(const uint32_t data, int const startBit, int const endBit)
{
	ValidateDataLength(data, endBit - startBit + 1);

	int tail = 8 - endBit % 8 - 1;
	int head = startBit % 8;
	int position = (endBit + tail) / 8;

	int headBitMask = (1 << head) - 1 << 8 - head;
	int tailBitMask = (1 << tail) - 1;
	int bitMask = headBitMask | tailBitMask;

	char oldData = bitMask & byteBuffer_->data()[position];

	const int last8Bitmask = (1 << 8) - 1;

	byteBuffer_->data()[position] = data << tail & last8Bitmask | oldData; 

	int forntHead = 8 - tail;
	int remainingBits = endBit - startBit - forntHead;
	int remainingData = data >> forntHead;
	position--;


	while (remainingBits >= 8 && position >= 0)
	{
		byteBuffer_->data()[position] = remainingData & last8Bitmask;

		position--;
		remainingData >>= 8;
		remainingBits -= 8;
	}

	if (remainingBits < 1)
		return;

	oldData = byteBuffer_->data()[position] & (1 << 8 - remainingBits) - 1 << remainingBits;
	byteBuffer_->data()[position] = oldData | remainingData & last8Bitmask;
}

uint32_t PacketStructure::ReadDataFromBuffer(int const startBit, int const endBit)
{
	int totalLength = (endBit - startBit) + 1;

	if (totalLength > 32)
		throw "Cannot read more then a unit32_t";

	int tail = 8 - startBit % 8;
	int position = (startBit + tail - 2) / 8;
	int length = tail;

	if (totalLength < length)
		length = totalLength;

	int bitMask = (1 << length) - 1;

	if (totalLength < tail)
		bitMask <<= tail - totalLength;

	int headData = byteBuffer_->data()[position] & bitMask;

	if (totalLength < tail)
	{
		headData >>= tail - totalLength;
	}

	int remainingBits = totalLength - length;
	position += 1;

	while (remainingBits > 7)
	{
		headData <<= 8;
		headData = headData | byteBuffer_->data()[position];
		remainingBits -= 8;
		position += 1;
	}

	if (remainingBits < 1)
		return headData;

	int remainder = 8 - remainingBits;
	headData <<= remainingBits;
	int bufferData = byteBuffer_->data()[position];
	bufferData >>= remainder;
	headData |= bufferData;

	return headData;
}


PacketStructure::~PacketStructure()
{
	delete byteBuffer_;
	byteBuffer_ = nullptr;
}

std::vector<uint8_t>* PacketStructure::GetBuffer() const
{
	return byteBuffer_;
}

int PacketStructure::getSizeByte()
{
	return sizeByte_;
}
