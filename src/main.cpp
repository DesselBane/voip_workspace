/******************************************************************************/
/*
 *  Project: Voice-over-IP
 *  Author: Matthias Tessmann <matthias.tessmann@th-nuernberg.de
 *  Date: October, 5th 2015
 *
 *  Contents: Application entry point.
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

#include "voip_comm.h"
#include "receiver.h"
#include "sender.h"
#include "rtpPackage.h"

using namespace std;

int main(int argc, char *argv[]) {
  
	auto pkg = RtpPackage();
	pkg.set_use_extension_headers(true)
		->set_payload_type(5)
		->set_sequence_number(10)
		->set_timestamp(50)
		->set_synchronization_source_identifier(100)
		->set_contributing_source_identifiers(128)
		->set_contributing_source_identifiers(32768)
		->set_contributing_source_identifiers(8388608)
		->set_contributing_source_identifiers(2147483648)
		->set_payload(new vector<uint8_t>{ 'T','e','s','t' });

	auto byteArray = pkg.Build();

	for(int i = 0; i < pkg.getSizeByte(); i++)
	{
		int charCode = byteArray->data()[i];
	}

	auto parsedPkg = RtpPackage::ParsePackage(byteArray);

	return 0;

	Receiver myReceiver;

	myReceiver.start();

	Sender mySender;

	while (true)
	{
		cout << endl << "Type your message:" << endl;

		string message;
		getline(cin, message);

		if (message == "exit")
			break;

		mySender.send(vector<uint8_t>(message.begin(), message.end()));
	}


	myReceiver.stop();
	
	return 0;
	
	
	
	VoIPComm communicator;
  return communicator.exec(argc, argv);
}
