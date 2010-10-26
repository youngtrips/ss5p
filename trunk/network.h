#ifndef __D_NETWORK_H
#define __D_NETWORK_H

#include <stdint.h>

typedef struct
{
    int listen_fd;
    int protocol;
    struct sockaddr *sockaddr;
    socklen_t socklen;
    char *add_str;
    uint16_t listen_port;
    int listen_backlog;
}listen_t;

int listen_init(int protocol, const char *ipaddr, unsigned short port, int backlog);

#endif
