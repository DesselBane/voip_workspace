#pragma once
#include "rtpPackage.h"

class RtpPackageProvider
{
public:
	virtual RtpPackage const* GetNextRtpPackage() = 0;

protected:
	RtpPackageProvider()
	{
	};

	virtual ~RtpPackageProvider()
	{
	};
};
