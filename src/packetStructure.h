#pragma once
#include <vector>

class PacketStrcutre
{
		char* byteBuffer_ = nullptr;
		int sizeByte_ = -1;

		void ValidateDataLength(int const data, int const length);

protected:

		void CreateByteBuffer(int const size);
		void WriteDataToBuffer( const uint32_t data, int const beginn, int const end );
		uint32_t ReadDataFromBuffer( int const startBit, int const endBit );

	public:
	virtual ~PacketStrcutre();
		virtual char* Build ( ) = 0;
		char* getBuffer();
		int getSizeByte();
};
