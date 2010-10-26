#ifndef __GLOBAL_H
#define __GLOBAL_H

#define MAXN_EVENTS 1024
#define MAXN_BUFFER 65535 

#include <stdint.h>

#include "buffer.h"

typedef struct event_t
{
    void *data;

    int ep;
    int fd;

    buffer_t *rbuf;
    buffer_t *wbuf;

    uint8_t ready_r;
    uint8_t ready_w;
    uint8_t status;

    void (*handle)(struct event_t *ev);
}fd_event_t;


#endif
