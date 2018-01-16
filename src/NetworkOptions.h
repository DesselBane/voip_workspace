#pragma once

#include "socket.h"

class NetworkOptions
{
public:
	explicit NetworkOptions(std::string destinationAddress, uint16_t destinationPort, std::string sourceAddress, uint16_t sourcePort);
	virtual ~NetworkOptions();


	util::Ipv4SocketAddress * GetDestinationIp() const;
	util::Ipv4SocketAddress * GetSourceIp() const;
private:
	util::Ipv4SocketAddress* destinationIp_ = nullptr;
	util::Ipv4SocketAddress* sourceIp_ = nullptr;
};