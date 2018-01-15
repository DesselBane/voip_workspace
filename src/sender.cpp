/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#include "Sender.h"
#include <iostream>

using namespace std;


Sender::Sender(RtpPackageProvider* provider)
{
	provier_ = provider;
	isSendingMutex_ = new mutex();
}

Sender::~Sender()
{
	if (IsSending())
		StopSending();

	provier_ = nullptr;
	if (isSendingMutex_ != nullptr)
	{
		delete isSendingMutex_;
		isSendingMutex_ = nullptr;
	}
}

void Sender::StartSending(util::Ipv4SocketAddress const* sendToAddress)
{
	if (IsSending())
		return;

	lock_guard<mutex> isSendingGuard(*isSendingMutex_);
	isSending_ = true;

	sendToAddress_ = sendToAddress;

	socket_ = new util::UdpSocket();
	socket_->open();

	workerThread_ = new thread([&] { SendLoop(); });
}

void Sender::StopSending()
{
	if (!IsSending())
		return;

	{
		lock_guard<mutex> isSendingGuard(*isSendingMutex_);
		isSending_ = false;
	}

	workerThread_->join();

	delete workerThread_;
	workerThread_ = nullptr;

	socket_->close();
	delete socket_;
	socket_ = nullptr;

	sendToAddress_ = nullptr;
}

bool Sender::IsSending()
{
	lock_guard<mutex> isSendingGuard(*isSendingMutex_);
	return isSending_;
}

void Sender::Send(const vector<uint8_t>& data)
{
	socket_->sendto(*sendToAddress_, data);
}

void Sender::SendLoop()
{
	while (IsSending())
	{
		auto pkg = provier_->GetNextRtpPackage();
		Send(*pkg->GetBuffer());
		delete pkg;
	}
}
