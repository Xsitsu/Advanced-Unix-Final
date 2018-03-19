#ifndef _PACKET_H
#define _PACKET_H

#define REQUEST_ADD 1
#define REQUEST_SUB 2

#define RESPONSE_VALID 1
#define RESPONSE_ERROR 2

typedef struct
{
	uint32_t x;
	uint32_t y;
	uint8_t op;

} request_t;

typedef struct
{
	uint32_t result;
	uint8_t status;
} response_t;

#endif
