#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>

#include "global.h"
#include "buffer.h"

#define BUFFER_SIZE(buffer) \
	(((buffer)->last) - ((buffer)->first))

void *buffer_init(size_t size)
{
    buffer_t *buffer = NULL;
    if((buffer = (buffer_t*)malloc(sizeof(buffer_t))) == NULL)
    {
        fprintf(stderr, "buffer_init() malloc error:%s\n", strerror(errno));
        return NULL;
    }
	if((buffer->data = (uint8_t*)malloc(sizeof(uint8_t) * size)) == NULL)
	{
		free(buffer);
		return NULL;
	}
    memset(buffer->data, 0, sizeof(uint8_t) * size);
    buffer->first = 0;
    buffer->last = 0;
    buffer->size = size * sizeof(uint8_t);
    return (void*)buffer;
}

void buffer_push(buffer_t *buffer, void *data, size_t size)
{
	uint8_t *new_buf = NULL;
	size_t tot; 
	size_t cap;
	if(buffer->last + size > buffer->size)
	{
		cap = sizeof(uint8_t) * buffer->last + size;
		new_buf = (uint8_t*)malloc(cap);
		tot = BUFFER_SIZE(buffer);
		memcpy(new_buf, (void*)(buffer->data + buffer->first), tot);
		free(buffer->data);
		buffer->data = new_buf;
		buffer->first = 0;
		buffer->last = tot;
		buffer->size = cap;
	}
    memcpy((void*)(buffer->data + buffer->last), data, size);
    buffer->last += size;
}

void buffer_free(buffer_t *buffer)
{
    if(buffer)
        free(buffer);
}

void buffer_clear(buffer_t *buffer)
{
    memset(buffer->data, 0, buffer->size);
    buffer->first = 0;
    buffer->last = 0;
}
