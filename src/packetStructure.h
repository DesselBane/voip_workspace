#pragma once
#include <vector>

using namespace std;

class PacketStructure
{
protected:
	vector<uint8_t>* byteBuffer_ = nullptr;
	int sizeByte_ = -1;

	void ValidateDataLength(int const data, int const length);

	void CreateByteBuffer(int const size);
	void WriteDataToBuffer(const uint32_t data, int const beginn, int const end);
	uint32_t ReadDataFromBuffer(int const startBit, int const endBit);


public:
	virtual ~PacketStructure();
	virtual vector<uint8_t>* Build() = 0;
	vector<uint8_t>* GetBuffer() const;
	int getSizeByte();
};
