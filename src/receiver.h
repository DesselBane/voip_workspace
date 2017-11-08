/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#ifndef VOIP_RECEIVER_H
#define VOIP_RECEIVER_H

#include <thread>
#include <iostream>
#include <socket.h>

class Receiver {
public:
  Receiver();
  ~Receiver();

  void start();
  void stop();

private:
  void receive();

  std::thread self_;
  bool        running_;
  util::UdpSocket* socket_;
  util::Ipv4SocketAddress* receiveAddress_;
};

#endif /* VOIP_RECEIVER_H */
