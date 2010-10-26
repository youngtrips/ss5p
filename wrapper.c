#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

#include "wrapper.h"

ssize_t xwrite(int fd, const void *buf, size_t count)
{
	size_t offset;
	ssize_t size;

	size = -1;
	offset = 0;
	while ((offset != count) && (size != 0)) 
	{
		size = write(fd, (char *)buf + offset, count - offset);
		if(size < 0)	
		{
			if(errno == EINTR)
				return -1;
			if(errno == EAGAIN)
			{
				usleep(1000);
				continue;
			}
			return -1;
		}
		
		offset += size;
	}

	return offset;
}

ssize_t xread(int fd, void *buf, size_t count)
{
	size_t offset;
	ssize_t size;

	size = -1;
	offset = 0;
	while ((offset != count) && (size != 0)) 
	{
		size = read(fd, (char *)buf + offset, count - offset);

		if(size == 0)
			break;
		if(size < 0)
		{
			if(errno == EAGAIN)
				break;
			return -1;
		}
		offset += size;
	}

	return offset;
}
