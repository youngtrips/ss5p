#ifndef __BUFFER_H
#define __BUFFER_H

#include <sys/types.h>
#include <stdint.h>

#include "global.h"

typedef struct
{
    uint8_t *data;
	
    size_t first;
    size_t last;
    size_t size;
}buffer_t;

#define BUFFER_SIZE(buffer) \
        (((buffer)->last) - ((buffer)->first))


void *buffer_init(size_t size);

void buffer_push(buffer_t *buffer, void *data, size_t size);

void buffer_clear(buffer_t *buffer);

void buffer_free(buffer_t *buffer);

#endif
