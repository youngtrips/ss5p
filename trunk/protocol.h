#ifndef __SOCKET5_H
#define __SOCKET5_H

#include <stdint.h>

#include "global.h"

struct ver_method_req_t
{
	uint8_t ver;
	uint8_t nmethonds;
	uint8_t methods[255];
};

struct ver_method_resp_t
{
	uint8_t ver;
	uint8_t method;
};

struct socket5_req_t
{
	uint8_t version;
	uint8_t comand;
	uint8_t reserved;
	uint8_t addr_type;
	uint8_t addr_port[1];
};

struct socket5_resp_t
{
	uint8_t version;
	uint8_t reply;
	uint8_t reserved;
	uint8_t addr_type;
	uint8_t addr_port[1];
};

#define BV(x) (1<<(x))

#define STAT_VER_METHOD_REQUEST	BV(1)
#define STAT_SOCKET5_REQUEST	BV(2)
#define STAT_PROCESS_DATA		BV(3)
#define STAT_ERROR				BV(4)


void process_ver_method_request(fd_event_t *ev);

void process_socket5_request(fd_event_t *ev);

void process_data(fd_event_t *ev);

#endif
