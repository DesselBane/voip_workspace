#include "NetworkOptions.h"

using namespace std;

NetworkOptions::NetworkOptions(string destinationAddress,
                               uint16_t destinationPort,
                               string sourceAddress,
                               uint16_t sourcePort)
{
	destinationIp_ = new util::Ipv4SocketAddress(destinationAddress, destinationPort);
	sourceIp_ = new util::Ipv4SocketAddress(sourceAddress, sourcePort);
}

NetworkOptions::~NetworkOptions()
{
}

util::Ipv4SocketAddress * NetworkOptions::GetDestinationIp() const
{
	return destinationIp_;
}

util::Ipv4SocketAddress * NetworkOptions::GetSourceIp() const
{
	return sourceIp_;
}
