#pragma once
#include <vector>

class PacketStrcutre
{
protected:
	std::vector<uint8_t>* byteBuffer_ = nullptr;
	int sizeByte_ = -1;

	void ValidateDataLength(int const data, int const length);

	void CreateByteBuffer(int const size);
	void WriteDataToBuffer(const uint32_t data, int const beginn, int const end);
	uint32_t ReadDataFromBuffer(int const startBit, int const endBit);


public:
	virtual ~PacketStrcutre();
	virtual std::vector<uint8_t>* Build() = 0;
	std::vector<uint8_t>* GetBuffer() const;
	int getSizeByte();
};
