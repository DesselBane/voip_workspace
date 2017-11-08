/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#ifndef VOIP_SENDER_H
#define VOIP_SENDER_H
#include <vector>
#include <socket.h>

class Sender
{
public:
	Sender();
	~Sender();
	void send(const std::vector<uint8_t>& data);

private:
	util::UdpSocket* socket_;
	util::Ipv4SocketAddress* sendToAddress_;
};

#endif /* VOIP_SENDER_H */
