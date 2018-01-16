#pragma once

class NetworkOptions
{
public:
	explicit NetworkOptions(string destinationAddress, uint16_t destinationPort, string sourceAddress, uint16_t sourcePort);
	virtual ~NetworkOptions();

private:
	util::Ipv4SocketAddress* destinationIp_ = nullptr;
	util::Ipv4SocketAddress* sourceIp_ = nullptr;
};