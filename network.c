#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>

int listen_init(int protocol, const char *ipaddr, unsigned short port, int backlog)
{
    struct sockaddr_in bnd_addr;
    socklen_t socklen;
	int opt;
    int fd;

    if((fd = socket(AF_INET, protocol, 0)) < 0)
    {
        fprintf(stderr, "socket() error: %s @%s:%d\n", strerror(errno), __FILE__, __LINE__);
        goto ERROR;
    }

	opt = SO_REUSEADDR;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    setnonblocking(fd);

    memset(&bnd_addr, 0, sizeof(bnd_addr));
    bnd_addr.sin_family = AF_INET;
    if(!strcmp(ipaddr, "INADDR_ANY"))
        bnd_addr.sin_addr.s_addr = INADDR_ANY;
    else
        bnd_addr.sin_addr.s_addr = inet_addr(ipaddr);
    bnd_addr.sin_port = htons(port);

    if(bind(fd, (struct sockaddr*)&bnd_addr, sizeof(bnd_addr)) < 0)
    {
        fprintf(stderr, "bind() error: %s @%s:%d\n", strerror(errno), __FILE__, __LINE__);
        goto ERROR;
    }
    if(listen(fd, backlog) < 0)
    {
        fprintf(stderr, "listen() error: %s@%s:%d\n", strerror(errno), __FILE__, __LINE__);
        goto ERROR;
    }
    return fd;
ERROR:
    if(fd)
        close(fd);
    return -1;
}

