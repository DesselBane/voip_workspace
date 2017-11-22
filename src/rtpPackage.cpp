#include "rtpPackage.h"

int RtpPackage::get_version ( ) const { return version_; }
void RtpPackage::set_version ( int version ) { version_ = version; }
bool RtpPackage::is_use_extension_headers ( ) const { return useExtensionHeaders_; }

void RtpPackage::set_use_extension_headers ( bool use_extension_headers )
{
	useExtensionHeaders_ = use_extension_headers;
}

bool RtpPackage::is_marker_bit ( ) const { return markerBit_; }
void RtpPackage::set_marker_bit ( bool marker_bit ) { markerBit_ = marker_bit; }
uint16_t RtpPackage::get_sequence_number ( ) const { return sequenceNumber; }
void RtpPackage::set_sequence_number ( uint16_t sequence_number ) { sequenceNumber = sequence_number; }
uint32_t RtpPackage::get_timestamp ( ) const { return timestamp_; }
void RtpPackage::set_timestamp ( uint32_t timestamp ) { timestamp_ = timestamp; }
uint32_t RtpPackage::get_synchronization_source_identifier ( ) const { return synchronizationSourceIdentifier_; }

void RtpPackage::set_synchronization_source_identifier ( uint32_t synchronization_source_identifier )
{
	synchronizationSourceIdentifier_ = synchronization_source_identifier;
}

uint32_t RtpPackage::get_contributing_source_identifier ( ) const { return contributingSourceIdentifier_; }

void RtpPackage::set_contributing_source_identifier ( uint32_t contributing_source_identifier )
{
	contributingSourceIdentifier_ = contributing_source_identifier;
}

std::vector<char>* RtpPackage::get_payload ( ) const { return payload_; }
void RtpPackage::set_payload ( std::vector<char>* payload ) { payload_ = payload; }
