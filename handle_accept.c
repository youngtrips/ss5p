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

#include "handle_connect.h"
#include "protocol.h"
#include "buffer.h"
#include "global.h"

void handle_accept(fd_event_t *ev)
{
    struct sockaddr_in clt;
    struct epoll_event epoll_ev;
    socklen_t socklen;

    fd_event_t *clt_event;
    int clt_fd;
    int fd;
    int ep;

    fd = ev->fd;
    ep = ev->ep;

    memset(&clt, 0, sizeof(clt));
    socklen = sizeof(struct sockaddr);
    if((clt_fd = accept(fd, (struct sockaddr*)&clt, &socklen)) < 0)
    {
        fprintf(stderr, "accept() error: %s\n", strerror(errno));
        return;
    }
    printf("connection from: %s:%u\n", inet_ntoa(clt.sin_addr), ntohs(clt.sin_port));

    setnonblocking(clt_fd);
    
    clt_event = (fd_event_t*)malloc(sizeof(fd_event_t));

    clt_event->data = NULL;
    clt_event->fd = clt_fd;
    clt_event->ep = ep;
    clt_event->handle = handle_connection;
    clt_event->status = STAT_VER_METHOD_REQUEST;
    clt_event->rbuf = buffer_init(MAXN_BUFFER);
    clt_event->wbuf = buffer_init(MAXN_BUFFER);

    epoll_ev.data.ptr = clt_event;
    epoll_ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(ep, EPOLL_CTL_ADD, clt_fd, &epoll_ev);
}



