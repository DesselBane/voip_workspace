/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#include "receiver.h"
#include "RtpPackageProvider.h"

using namespace std;

Receiver::Receiver()
	: self_(),
	  running_(false)
{
}

Receiver::~Receiver()
{
	if (running_)
	{
		stop();
	}
}

void Receiver::start()
{
	if (running_)
		return;

	running_ = true;

	socket_ = new util::UdpSocket();

	receiveAddress_ = new util::Ipv4SocketAddress("0.0.0.0", 8888);

	socket_->open();

	socket_->bind(*receiveAddress_);

	self_ = std::thread([&] { receive(); });
}

void Receiver::stop()
{
	running_ = false;
	socket_->close();

	self_.join();
	delete socket_;
	delete receiveAddress_;
}

void Receiver::receive()
{
	static bool once = true;
	while (running_)
	{
		if (once)
		{
			cout << " #### Receiver: This is the receiver thread. Read packets from the network and ";
			cout << "push them into JB for further processing. Keep in mind that proper synchronization is necessary. ####" <<
				endl;
			once = false;
		}

		vector<uint8_t>* inputBuffer = new vector<uint8_t>(4096, 0);
		socket_->recvfrom(*receiveAddress_, *inputBuffer, 4096);

		auto pkg = RtpPackage::ParsePackage(inputBuffer);


		cout << "received pkg with version: " << pkg->get_version() << endl;

		delete inputBuffer;
		delete pkg;
	}
}
