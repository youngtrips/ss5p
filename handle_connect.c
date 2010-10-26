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

#include "protocol.h"
#include "global.h"

void handle_ev(fd_event_t *ev)
{
    switch(ev->status)
    {
        case STAT_VER_METHOD_REQUEST:
//            printf("version/method request\n");
            process_ver_method_request(ev);
            break;
        case STAT_SOCKET5_REQUEST:
//            printf("socket5 request\n");
            process_socket5_request(ev);
            break;
        case STAT_PROCESS_DATA:
//            printf("forward data\n");
            process_data(ev);
            break;
    }
}

void handle_connection(fd_event_t *ev)
{
    struct epoll_event epoll_ev;
    ssize_t size;
    int ep;
    int fd;
    uint8_t buf[MAXN_BUFFER]; 
    buffer_t *buffer;

    fd_event_t *opp_ev;

    fd = ev->fd;
    ep = ev->ep;

    opp_ev = (fd_event_t*)(ev->data);
    
    if(1 == ev->ready_w)
    {
        buffer = ev->wbuf;


        size = xwrite(ev->fd, (void*)(buffer->data + buffer->first), BUFFER_SIZE(buffer));
        if(size < 0)
        {
            fprintf(stderr, "xwrite() error...\n");
            return;
        }
        else if(size == 0)
        {
            printf("try to close...\n");
            
            epoll_ctl(ep, EPOLL_CTL_DEL, opp_ev->fd, NULL);
            epoll_ctl(ep, EPOLL_CTL_DEL, ev->fd, NULL);

            close(opp_ev->fd);
            close(ev->fd);
           
            buffer_free(opp_ev->rbuf);
            buffer_free(opp_ev->wbuf);
            buffer_free(ev->rbuf);
            buffer_free(ev->wbuf);
            
            free(opp_ev);
            free(ev);
            return;
        }
        else
        {
            if(size == BUFFER_SIZE(buffer))
            {
                buffer_clear(buffer);
            }
            else
            {
                buffer->first += size;
            }
        }

        epoll_ev.data.ptr = ev;
        epoll_ev.events = EPOLLIN | EPOLLET;
        epoll_ctl(ep, EPOLL_CTL_MOD, ev->fd, &epoll_ev);
    }
    if(1 == ev->ready_r)
    {
        buffer = ev->rbuf;
        memset(buf, 0, sizeof(buf));
        size = xread(ev->fd, (void*)buf, MAXN_BUFFER);
        if(size < 0)
        {
            fprintf(stderr, "xread() error...\n");
            return;
        }
        else if(size == 0)
        {
            printf("close....\n");
            epoll_ctl(ep, EPOLL_CTL_DEL, fd, NULL);
            epoll_ctl(ep, EPOLL_CTL_DEL, opp_ev->fd, NULL);
            close(fd);
            close(opp_ev->fd);
            buffer_free(ev->rbuf);
            buffer_free(ev->wbuf);
            buffer_free(opp_ev->rbuf);
            buffer_free(opp_ev->wbuf);
            free(opp_ev);
            free(ev);
            return;
        }
        else
        {
            buffer_push(buffer, buf, size);
        }
        handle_ev(ev);
    }
}

