#include "rtpPackage.h"

RtpPackage::RtpPackage ( ) {}
RtpPackage::~RtpPackage ( )
{
	delete contributingSourceIdentifiers;
	contributingSourceIdentifiers = nullptr;
}

char* RtpPackage::Build ( )
{
	const int baseByteSize = 12;
	const int contributerByteSize = contributingSourceIdentifiers->size() * 4;
	const int paddingRemainder = (32 - payload_->size() * 8 % 32) % 32;
	int padding = 0;
	int paddingSizeInByte = paddingRemainder / 8;

	int sizeByte = baseByteSize + contributerByteSize;
	sizeByte += payload_->size();

	if(paddingRemainder > 0)
	{
		padding = 1;
		sizeByte += paddingSizeInByte;
	}

	if (sizeByte * 8 % 32 != 0)
		throw "Calculations went wrong !!!";

	CreateByteBuffer(sizeByte);
	
	WriteDataToBuffer(version_, 0, 1);
	WriteDataToBuffer(padding, 2, 2);
	WriteDataToBuffer(useExtensionHeaders_, 3, 3);
	WriteDataToBuffer(contributingSourceIdentifiers->size(), 4, 7);
	WriteDataToBuffer(markerBit_, 8, 8);
	WriteDataToBuffer(payloadType_, 9, 15);
	WriteDataToBuffer(sequenceNumber_, 16, 31);
	WriteDataToBuffer(timestamp_, 32, 63);
	WriteDataToBuffer(synchronizationSourceIdentifier_, 64, 95);

	int position = 96;
	for ( unsigned identifier : *contributingSourceIdentifiers )
	{
		WriteDataToBuffer(identifier, position, position + 31);
		position += 32;
	}

	for ( char payload : *payload_ )
	{
		WriteDataToBuffer(payload, position, position + 7);
		position += 8;
	}

	if(padding > 0)
	{
		WriteDataToBuffer(paddingSizeInByte , sizeByte * 8 - 8, sizeByte * 8 - 1);
	}

	return GetByteBuffer();

}

std::vector<uint32_t> RtpPackage::get_contributing_source_identifiers ( ) const
{
	return *contributingSourceIdentifiers;
}

RtpPackage* RtpPackage::set_contributing_source_identifiers ( uint32_t ident )
{
	if (contributingSourceIdentifiers->size() > 14)
		throw "Can only have up to 15 contributors";

	contributingSourceIdentifiers->push_back(ident);
}


int RtpPackage::get_version ( ) const { return version_; }

RtpPackage* RtpPackage::set_version ( int version )
{
	version_ = version;
	return this;
}

bool RtpPackage::is_use_extension_headers ( ) const { return useExtensionHeaders_; }

RtpPackage* RtpPackage::set_use_extension_headers ( bool use_extension_headers )
{
	useExtensionHeaders_ = use_extension_headers;
	return this;
}

bool RtpPackage::is_marker_bit ( ) const { return markerBit_; }

RtpPackage* RtpPackage::set_marker_bit ( bool marker_bit )
{
	markerBit_ = marker_bit;
	return this;
}
uint16_t RtpPackage::get_sequence_number ( ) const { return sequenceNumber_; }
RtpPackage* RtpPackage::set_sequence_number ( uint16_t sequence_number )
{
	sequenceNumber_ = sequence_number;
	return this;
}
uint32_t RtpPackage::get_timestamp ( ) const { return timestamp_; }
RtpPackage* RtpPackage::set_timestamp ( uint32_t timestamp )
{
	timestamp_ = timestamp;
	return this;
}
uint32_t RtpPackage::get_synchronization_source_identifier ( ) const { return synchronizationSourceIdentifier_; }

RtpPackage* RtpPackage::set_synchronization_source_identifier ( uint32_t synchronization_source_identifier )
{
	synchronizationSourceIdentifier_ = synchronization_source_identifier;
	return this;
}


std::vector<char>* RtpPackage::get_payload ( ) const { return payload_; }
RtpPackage* RtpPackage::set_payload ( std::vector< char >* payload )
{
	payload_ = payload;
	return this;
}

int RtpPackage::get_payload_type ( ) const { return payloadType_; }
RtpPackage* RtpPackage::set_payload_type ( int payload_type )
{
	payloadType_ = payload_type;
	return this;
}
