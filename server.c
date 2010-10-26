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
#include "handle_accept.h"
#include "network.h"
#include "global.h"

fd_event_t *fd_event_array[MAXN_EVENTS * 3];
int tot;

int main()
{
    struct epoll_event event_list[MAXN_EVENTS];
    struct epoll_event ev;
    
    fd_event_t *fd_event;
    int listen_fd;
    uint32_t event;
    int nfds;
    int fd;
    int ep;
    int i;

    listen_fd = listen_init(SOCK_STREAM, "INADDR_ANY", 1080, 20);

    ep = epoll_create(MAXN_EVENTS / 2);

    fd_event = (fd_event_t*)malloc(sizeof(fd_event_t));
    fd_event->ep = ep;
    fd_event->fd = listen_fd;
    fd_event->handle = handle_accept;
    fd_event->rbuf = NULL;
    fd_event->rbuf = NULL;
    fd_event->data = NULL;

    ev.data.ptr = fd_event;
    ev.events = EPOLLIN | EPOLLET;

    epoll_ctl(ep, EPOLL_CTL_ADD, listen_fd, &ev);
    for(;;)
    {
        nfds = epoll_wait(ep, event_list, MAXN_EVENTS, 500);
        tot = 0; 
        for(i = 0;i < nfds; i++)
        {
            fd_event = event_list[i].data.ptr;
            event = event_list[i].events;
            
            fd_event->ready_r = 0;
            fd_event->ready_w = 0;
           
            if(event & (EPOLLOUT | EPOLLET))
            {
                fd_event->ready_w = 1;
                fd_event_array[tot++] = fd_event;
            }
            if(event & (EPOLLIN | EPOLLET))
            {
                fd_event->ready_r = 1;
                fd_event_array[tot++] = fd_event;
            }
        }

        for(i = 0;i < tot; i++)
        {
            fd_event = fd_event_array[i];
            if(fd_event == NULL)
                continue;
            fd_event->handle(fd_event);
            fd_event_array[i] = NULL;
        }
    }
    return 0;
}

