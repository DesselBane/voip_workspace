#pragma once

class PacketStrcutre
{
	private:
		char* byteBuffer_ = nullptr;
		int sizeByte_ = -1;

		void ValidateDataLength(int const data, int const length);

	protected:
		void CreateByteBuffer(int const size);
		void WriteDataToBuffer(int const data, int const beginn, int const end);

	public:
		~PacketStrcutre();
		virtual char* Build() = 0;
		char* getBuffer();
		int getSizeByte();
};