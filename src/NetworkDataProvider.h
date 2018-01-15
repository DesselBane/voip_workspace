#pragma once
#include <vector>

using namespace std;

class NetworkDataProvider
{
public:
	virtual vector<uint8_t>* GetNextDataPackage() = 0;

protected:
	NetworkDataProvider()
	{
	}

	virtual ~NetworkDataProvider()
	{
	}
};
