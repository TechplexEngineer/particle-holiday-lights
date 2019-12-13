/*
* Project: E131 - E.131 (sACN) library for Particle Devices
* Copyright (c) 2017 AdmiralTriggerHappy
* Dervived from work by:
* Copyright (c) 2015 Shelby Merrick
* http://www.forkineye.com
*
*  This program is provided free for you to use in any way that you wish,
*  subject to the laws and regulations where you are using it.  Due diligence
*  is strongly suggested before using this code.  Please give credit where due.
*
*  The Author makes no warranty of any kind, express or implied, with regard
*  to this program or the documentation contained in this document.  The
*  Author shall not be liable in any event for incidental or consequential
*  damages in connection with, or arising out of, the furnishing, performance
*  or use of these programs.
*

 */
#include "E131.h"

uint8_t sequence; /* Sequence tracker */

UDP udp;


/**
 * Constructor.
 */
E131::E131()
{
	// be sure not to call anything that requires hardware be initialized here, 
	// put those in begin()
}

/**
 * Example method.
 */
void E131::begin()
{
	// initialize hardware
	IPAddress multicast(239,255,0,1);
	udp.begin(E131_DEFAULT_PORT);
	udp.joinMulticast(multicast);
	packet = &packetBuffer;
}

void E131::begin(uint16_t _universe)
{
	// initialize hardware
	IPAddress multicast(239,255,((_universe >> 8) & 0xff),((_universe >> 0) & 0xff));
	udp.begin(E131_DEFAULT_PORT);
	udp.joinMulticast(multicast);
	packet = &packetBuffer;
}

/* Packet validater */
e131_error_t E131::validateE131Packet()
{
	if (memcmp(packet->acn_id, ACN_ID, sizeof(packet->acn_id)) !=0)
		return ERROR_ACN_ID;
	if (htonl(packet->root_vector) != VECTOR_ROOT)

		return ERROR_VECTOR_ROOT;
	if (htonl(packet->frame_vector) != VECTOR_FRAME)

		return ERROR_VECTOR_FRAME;
	if (packet->dmp_vector != VECTOR_DMP)

		return ERROR_VECTOR_DMP;
	return ERROR_NONE;
}

void E131::dumpError(e131_error_t error)
{
	switch (error) {
		case ERROR_ACN_ID:
			Serial.print(F("INVALID PACKET ID: "));
			for (uint8_t i = 0; i < sizeof(ACN_ID); i++)
				Serial.print(packet->acn_id[i], HEX);
			Serial.println("");
			break;
		case ERROR_PACKET_SIZE:
			Serial.println(F("INVALID PACKET SIZE: "));
			break;
		case ERROR_VECTOR_ROOT:
			Serial.print(F("INVALID ROOT VECTOR: 0x"));
			Serial.println(htonl(packet->root_vector), HEX);
			break;
		case ERROR_VECTOR_FRAME:
			Serial.print(F("INVALID FRAME VECTOR: 0x"));
			Serial.println(htonl(packet->frame_vector), HEX);
			break;
		case ERROR_VECTOR_DMP:
			Serial.print(F("INVALID DMP VECTOR: 0x"));
			Serial.println(packet->dmp_vector, HEX);
	}
}

uint16_t E131::parsePacket()
{
	e131_error_t error;
	uint16_t retval = 0;

	int size = udp.receivePacket(packet->raw, E131_PACKET_SIZE);
	if (size > 0)
	{
		error = validateE131Packet();

		if (!error)
		{
			universe = htons(packet->universe);
			data = packet->property_values;

			retval = htons(packet->property_value_count) - 1;
			if (packet->sequence_number != sequence++)
			{
				stats.sequence_errors++;
				sequence = packet->sequence_number + 1;
			}
			stats.num_packets++;
		}
		else
		{
			dumpError(error);
			stats.packet_errors++;
			
		}
	}

	return retval;
}
