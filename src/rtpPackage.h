#pragma once

#include "PacketStructure.h"
#include <cstdint>
#include <vector>

using namespace std;

class RtpPackage : public PacketStructure
{
	int version_ = 2;
	int useExtensionHeaders_ = 0;
	int markerBit_ = 0;
	uint16_t sequenceNumber_ = 0;
	uint32_t timestamp_ = 0;
	uint32_t synchronizationSourceIdentifier_ = 0;
	vector<uint32_t>* contributingSourceIdentifiers = new vector<uint32_t>();
	vector<uint8_t>* payload_ = nullptr;
	int payloadType_ = 0;
	static const int base_byte_size = 12;

public:
	RtpPackage();
	~RtpPackage();

	vector<uint8_t>* Build() override;

	vector<uint32_t> get_contributing_source_identifiers() const;
	RtpPackage* set_contributing_source_identifiers(uint32_t ident);

	int get_version() const;
	RtpPackage* set_version(int version);

	bool is_use_extension_headers() const;
	RtpPackage* set_use_extension_headers(bool use_extension_headers);

	bool is_marker_bit() const;
	RtpPackage* set_marker_bit(bool marker_bit);

	uint16_t get_sequence_number() const;
	RtpPackage* set_sequence_number(uint16_t sequence_number);

	uint32_t get_timestamp() const;
	RtpPackage* set_timestamp(uint32_t timestamp);

	uint32_t get_synchronization_source_identifier() const;
	RtpPackage* set_synchronization_source_identifier(uint32_t synchronization_source_identifier);

	vector<uint8_t>* get_payload() const;
	RtpPackage* set_payload(vector<uint8_t>* payload);

	int get_payload_type() const;
	RtpPackage* set_payload_type(int payload_type);

	bool IsValid();

	static RtpPackage* ParsePackage(const vector<uint8_t>* byteArray);
};
