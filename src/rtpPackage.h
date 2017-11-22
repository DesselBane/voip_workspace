#pragma once

#include "packetStructure.h"
#include <cstdint>
#include <vector>

class RtpPackage : public PacketStrcutre
{
	int version_ = 2;
	bool useExtensionHeaders_ = false;
	bool markerBit_ = false;
	uint16_t sequenceNumber = 0;
	uint32_t timestamp_ = 0;
	uint32_t synchronizationSourceIdentifier_ = 0;
	uint32_t contributingSourceIdentifier_ = 0;
	std::vector<char>* payload_ = nullptr;

	public:
	char* Build ( ) override;

	int get_version ( ) const;
	void set_version ( int version );
	bool is_use_extension_headers ( ) const;
	void set_use_extension_headers ( bool use_extension_headers );
	bool is_marker_bit ( ) const;
	void set_marker_bit ( bool marker_bit );
	uint16_t get_sequence_number ( ) const;
	void set_sequence_number ( uint16_t sequence_number );
	uint32_t get_timestamp ( ) const;
	void set_timestamp ( uint32_t timestamp );
	uint32_t get_synchronization_source_identifier ( ) const;
	void set_synchronization_source_identifier ( uint32_t synchronization_source_identifier );
	uint32_t get_contributing_source_identifier ( ) const;
	void set_contributing_source_identifier ( uint32_t contributing_source_identifier );
	std::vector<char>* get_payload ( ) const;
	void set_payload ( std::vector<char>* payload );
};


