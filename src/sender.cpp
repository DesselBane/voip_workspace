/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#include "sender.h"
#include <iostream>

using namespace std;

Sender::Sender()
{
	socket_ = new util::UdpSocket();
	socket_->open();
	sendToAddress_ = new util::Ipv4SocketAddress("127.0.0.1", 8888);
}

Sender::~Sender()
{
	delete socket_;
	delete sendToAddress_;
}

void Sender::send(const vector<uint8_t>& data)
{

	socket_->sendto(*sendToAddress_, data);
}
