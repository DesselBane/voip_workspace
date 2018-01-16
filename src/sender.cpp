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


Sender::Sender(RtpPackageProvider* provider, NetworkOptions* networkOptions)
{
	provier_ = provider;
	isSendingMutex_ = new mutex();
	networkOptions_ = networkOptions;
}

Sender::~Sender()
{
	if (IsSending())
		StopSending();

	provier_ = nullptr;
	networkOptions_ = nullptr;

	if (isSendingMutex_ != nullptr)
	{
		delete isSendingMutex_;
		isSendingMutex_ = nullptr;
	}
}

void Sender::StartSending()
{
	if (IsSending())
		return;

	cout << "Starting to send" << endl;

	lock_guard<mutex> isSendingGuard(*isSendingMutex_);
	isSending_ = true;

	socket_ = new util::UdpSocket();
	socket_->open();

	workerThread_ = new thread([&] { SendLoop(); });
}

void Sender::StopSending()
{
	if (!IsSending())
		return;

	cout << "Stopping to send";

	{
		lock_guard<mutex> isSendingGuard (*isSendingMutex_);
		isSending_ = false;
	}

	workerThread_->join();

	delete workerThread_;
	workerThread_ = nullptr;

	socket_->close();
	delete socket_;
	socket_ = nullptr;

	cout << "   ...done" << endl;
}

bool Sender::IsSending()
{
	lock_guard<mutex> isSendingGuard(*isSendingMutex_);
	return isSending_;
}

void Sender::Send(const vector<uint8_t>& data)
{
	socket_->sendto(*networkOptions_->GetDestinationIp(), data);
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
