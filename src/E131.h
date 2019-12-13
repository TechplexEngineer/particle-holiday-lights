/*
* E131.h
*
* Project: E131 - E.131 (sACN) library for Photon
* Copyright (c) 2017 AdmiralTriggerHappy
* Derived from work from: 
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

// This will load the definition for common Particle variable types
#include "Particle.h"

#ifndef E131_h
#define E131_h

#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)

#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
((x)<< 8 & 0x00FF0000UL) | \
((x)>> 8 & 0x0000FF00UL) | \
((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)

/* Defaults */
#define E131_DEFAULT_PORT 5568
#define WIFI_CONNECT_TIMEOUT 10000  /* 10 seconds */

/* E1.31 Packet Offsets */
#define E131_ROOT_PREAMBLE_SIZE 0
#define E131_ROOT_POSTAMBLE_SIZE 2
#define E131_ROOT_ID 4
#define E131_ROOT_FLENGTH 16
#define E131_ROOT_VECTOR 18
#define E131_ROOT_CID 22

#define E131_FRAME_FLENGTH 38
#define E131_FRAME_VECTOR 40
#define E131_FRAME_SOURCE 44
#define E131_FRAME_PRIORITY 108
#define E131_FRAME_RESERVED 109
#define E131_FRAME_SEQ 111
#define E131_FRAME_OPT 112
#define E131_FRAME_UNIVERSE 113

#define E131_DMP_FLENGTH 115
#define E131_DMP_VECTOR 117
#define E131_DMP_TYPE 118
#define E131_DMP_ADDR_FIRST 119
#define E131_DMP_ADDR_INC 121
#define E131_DMP_COUNT 123
#define E131_DMP_DATA 125

#define E131_PACKET_SIZE 638

/* Status structure */
typedef struct
{
	uint32_t    num_packets;
	uint32_t    sequence_errors;
	uint32_t    packet_errors;
} e131_stats_t;

/* E1.31 Packet Structure */
typedef union
{
	struct
	{
		/* Root Layer */
		uint16_t preamble_size;
		uint16_t postamble_size;
		char	 acn_id[12];
		uint16_t root_flength;
		uint32_t root_vector;
		uint8_t  cid[16];

		/* Frame Layer */
		uint16_t frame_flength;
		uint32_t frame_vector;
		uint8_t  source_name[64];
		uint8_t  priority;
		uint16_t reserved;
		uint8_t  sequence_number;
		uint8_t  options;
		uint16_t universe;

		/* DMP Layer */
		uint16_t dmp_flength;
		uint8_t  dmp_vector;
		uint8_t  type;
		uint16_t first_address;
		uint16_t address_increment;
		uint16_t property_value_count;
		uint8_t  property_values[513];
	} __attribute__((packed));

	uint8_t raw[E131_PACKET_SIZE];
} e131_packet_t;

/* Error Types */
typedef enum
{
	ERROR_NONE,
	ERROR_ACN_ID,
	ERROR_PACKET_SIZE,
	ERROR_VECTOR_ROOT,
	ERROR_VECTOR_FRAME,
	ERROR_VECTOR_DMP
} e131_error_t;

/* Constants for packet validation */
static const char ACN_ID[12] = { 0x41, 0x53, 0x43, 0x2d, 0x45, 0x31, 0x2e, 0x31, 0x37, 0x00, 0x00, 0x00 };
static const uint32_t VECTOR_ROOT = 4;
static const uint32_t VECTOR_FRAME = 2;
static const uint8_t VECTOR_DMP = 2;

// This is your main class that users will import into their application
class E131
{
	public:

		uint8_t *data; // Pointer to DMX channel data
		uint16_t universe; // DMX Universe of last valid packet
		e131_stats_t  stats; // Statistics tracker

		/**
		* Constructor
		*/
		E131();


		void begin();
		void begin(uint16_t universeIP);
		uint16_t parsePacket();

	private:

		e131_packet_t packetBuffer; // Packet buffer
		e131_packet_t *packet; // Pointer to last valid packet
		e131_error_t validateE131Packet();
		void dumpError(e131_error_t error);
 
};

#endif //E131_h
