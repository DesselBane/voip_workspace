/******************************************************************************/
/*
 *  Project: Voice-over-IP
 *  Author: Matthias Tessmann <matthias.tessmann@th-nuernberg.de
 *  Date: October, 5th 2015
 *
 *  Contents: Application wrapper class skeleton.
 *            Intended for educational purposes.
 *
 *  Additoinal notes:
 *         - There is very little error handling.
 *         - The implementation may contain bugs of any kind!
 *
 * The MIT License (MIT)
 *
 *  Copyright (c) 2015 Matthias Teßmann <matthias.tessmann@th-nuernberg.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
/******************************************************************************/

#include <iostream>

#include "soundcard.h"

#include "receiver.h"

#include "tclap/CmdLine.h"
#include "sender.h"
#include "AudioManager.h"
#include "VoipComm.h"
#include "RtpPacker.h"
#include "RtpDepacker.h"

using namespace std;


VoIPComm::VoIPComm()
{
}

VoIPComm::~VoIPComm()
{
}

int VoIPComm::exec(int argc, char* argv[])
{
	if (!Init(argc, argv))
	{
		cerr << "Error initializing!" << endl;
		return -1;
	}

	// Init soundcard and stuff - you can also do this in init.
	// Wire up all required connections.
	// Then somewhere, start the receiver (this can also become a member variable - its entirely up to you!)
	//Create Object Graph
	auto receiver = new Receiver(networkOptions_);
	auto depacker = new RtpDepacker(receiver,audioOptions_);

	auto audioManager = new AudioManager(depacker);
	util::SoundCard mySoundCard(audioManager);
	auto packer = new RtpPacker(audioManager, rtpOptions_);
	auto sender = new Sender(packer, networkOptions_);


	//Configure
	mySoundCard.init(audioOptions_->GetInputDevice(),
	                 audioOptions_->GetOutputDevice(),
	                 audioOptions_->GetInputChannels(),
	                 audioOptions_->GetOutputChannels(),
	                 audioOptions_->GetSampleRate(),
	                 audioOptions_->GetFrameSize(),
	                 audioOptions_->GetFormat());


	//Start
	receiver->Start();
	depacker->StartUnpacking();

	audioManager->StartRecording();
	packer->StartPacking();
	sender->StartSending();
	mySoundCard.start();

	// Just wait for enter
	char input;
	cin.get(input);

	//Stop
	sender->StopSending();
	packer->StopPacking();
	audioManager->StopRecording();
	receiver->Stop();
	depacker->StopUnpacking();
	mySoundCard.stop();


	//Cleanup
	delete sender;
	delete packer;
	delete audioManager;
	delete receiver;

	delete rtpOptions_;
	delete audioOptions_;
	delete networkOptions_;

	cout << "Program finished press <enter> to exit" << endl;
	cin.get(input);

	return 0;
}

bool VoIPComm::Init(int argc, char* argv[])
{
	srand(TIME_UTC);

	/* Set up command line arguments */
	try
	{
		TCLAP::CmdLine cmd("VoIP Real-Time Communication Project", ' ', "0.1");

		// This is the only command line argument currently used
		TCLAP::SwitchArg listDevices("l", "list-devices", "List audio devices", cmd, false);

		// These here show you what you might need
		TCLAP::ValueArg<int> inputDevice("i", "input-device", "Select input device", false, -1, "int", cmd);
		TCLAP::ValueArg<int> outputDevice("o", "output-device", "Select output device", false, -1, "int", cmd);
		TCLAP::ValueArg<unsigned int> inputChannelNumber("", "inCh", "Number of input channels (default: 1)", false, 1,
		                                                 "unsigned int", cmd);
		TCLAP::ValueArg<unsigned int> outputChannelNumber("", "outCh", "Number of output channels (default 1)", false, 1,
		                                                  "unsigned int", cmd);
		TCLAP::ValueArg<unsigned int> frameSize("f", "Framesize", "Framesize (default: 512)", false, 512, "unsigned int",
		                                        cmd);
		TCLAP::ValueArg<unsigned int> sampleRate("s", "samplerate", "Samplerate (default: 44100)", false, 44100,
		                                         "unsigned int", cmd);
		TCLAP::ValueArg<unsigned int> destinationPort("", "rPort", "Remote Port (default: 1976)", false, 1976, "unsigned int",
		                                              cmd);
		TCLAP::ValueArg<unsigned int> sourcePort("", "lPort", "Local Port (default: 1976)", false, 1976, "unsigned int", cmd);

		TCLAP::UnlabeledValueArg<string> destinationIp("destIp", "Destination IP address", false, "", "std::string", cmd);
		TCLAP::UnlabeledValueArg<string> sourceIp("sourceIp", "Source IP address", false, "0.0.0.0", "std::string", cmd);

		cmd.parse(argc, argv);

		/* Add argument processing here */
		if (listDevices.getValue())
		{
			this->listDevices();
			exit(0);
		}

		// if -l is not specified, the IP is mandatory
		// in order to establish an endpoint connection
		if (destinationIp.getValue() == "")
		{
			TCLAP::StdOutput().usage(cmd);
			exit(-1);
		}

		if (sampleRate.getValue() != 44100)
		{
			cerr << "BETA Restriction: Can only operate with a sample rate of 44100" << endl;
			exit(-2);
		}

		if (inputChannelNumber.getValue() != 1)
		{
			cerr << "BETA Restriction: Can only use 1 input channel. 1 channel will be used" << endl;
		}

		audioOptions_ = new AudioOptions(inputDevice.getValue(), outputDevice.getValue(), frameSize.getValue(),
		                                 sampleRate.getValue());
		networkOptions_ = new NetworkOptions(destinationIp.getValue(), destinationPort.getValue(), sourceIp.getValue(),
		                                     sourcePort.getValue());
		rtpOptions_ = new RtpOptions(audioOptions_, 11);
	}
	catch (TCLAP::ArgException& argEx)
	{
		cerr << "Error parsing command line arguments: " << argEx.error() << " for argument " << argEx.argId() << endl;
		return false;
	}

	return true;
}

void VoIPComm::listDevices()
{
	util::SoundCard::listDevices();
}
