#ifndef PACKETPROVIER_H
#define PACKETPROVIER_H
#include <audiobuffer.h>

class PacketProvider
{
public:
	util::AudioBuffer* getPackage();
};
#endif // PACKETPROVIER_H
