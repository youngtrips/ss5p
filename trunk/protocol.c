#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <unistd.h>

#include "handle_connect.h"
#include "protocol.h"
#include "buffer.h"
#include "global.h"

void process_ver_method_request(fd_event_t *ev)
{
    unsigned char buf[MAXN_BUFFER];
	struct ver_method_req_t *req = NULL;
	struct ver_method_resp_t resp;
    int ret;
    struct epoll_event epoll_ev;
    buffer_t *rbuf;
    buffer_t *wbuf;
    
    rbuf = ev->rbuf;
    wbuf = ev->wbuf;

    //copy data from read buffer
    memset(buf, 0, sizeof(buf));
    memcpy(buf, (void*)(rbuf->data + rbuf->first), BUFFER_SIZE(rbuf));
    buffer_clear(rbuf);

    req = (struct ver_method_req_t*)buf;
	memset(&resp, 0, sizeof(resp));
	resp.ver = 0x05;
	resp.method = 0x00;

    //copy data to write buffer
    buffer_push(wbuf, (void*)&resp, sizeof(resp));

    epoll_ev.data.ptr = ev;
    epoll_ev.events = EPOLLOUT | EPOLLET;
    epoll_ctl(ev->ep, EPOLL_CTL_MOD, ev->fd, &epoll_ev);
    ev->status = STAT_SOCKET5_REQUEST;
}

void process_socket5_request(fd_event_t *ev)
{
    uint8_t buf[MAXN_BUFFER];
	uint8_t msg[16];
	struct socket5_req_t *req = NULL;
	struct socket5_resp_t *resp = NULL;
	uint32_t srv_ip;
	uint16_t srv_port;
	uint32_t *ptr_ip;
	uint16_t *ptr_port;
	struct sockaddr_in srv_addr;
	int conn_fd;
	int ret;
    struct epoll_event epoll_ev;

	struct hostent *phost;
	struct in_addr *sin_addr;

	char domain[256];
	uint8_t domain_len;

    fd_event_t *srv_ev;

    buffer_t *rbuf;
    buffer_t *wbuf;

	conn_fd = 0;
	srv_ip = 0;
	srv_port = 0;

    rbuf = ev->rbuf;
    wbuf = ev->wbuf;

    //copy data from read buffer 
    memset(buf, 0, sizeof(buf));
    memcpy(buf, (void*)(rbuf->data + rbuf->first), BUFFER_SIZE(rbuf));
    buffer_clear(rbuf);

	req = (struct socket5_req_t*)buf;
	if(req->addr_type == 0x01) // ipv4
	{
		srv_ip = *((uint32_t*)req->addr_port);
		srv_port = *((uint16_t*)(req->addr_port + 4));
	}
	else if(req->addr_type == 0x03) //domain
	{
		domain_len = *((uint8_t*)(&(req->addr_type) + 1));
		memset(domain, 0, sizeof(domain));
		strncpy(domain, (char*)(&(req->addr_type) + 2), domain_len);
		
		printf("domain: [%s]\n", domain);
		phost = (struct hostent*)gethostbyname(domain);
		if(phost == NULL)
		{
			printf("Resolve %s error!\n" , domain );
			goto ERROR;
		}
		sin_addr = (struct in_addr*)(phost->h_addr);
		srv_ip = sin_addr->s_addr;
		srv_port = *((uint16_t*)(&(req->addr_type) + 2 + domain_len));
	}

	conn_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = srv_ip;
	srv_addr.sin_port = srv_port;

	printf("target server: %s:%u\n", inet_ntoa(srv_addr.sin_addr), ntohs(srv_port));

	memset(msg, 0, sizeof(msg));
	resp = (struct socket5_resp_t*)msg;
	ptr_ip = (uint32_t*)(resp->addr_port);
	ptr_port = (uint16_t*)(resp->addr_port + 4);

	if(connect(conn_fd, (struct sockaddr*)&srv_addr, sizeof(struct sockaddr)) < 0)
	{
		printf("socket request error...\n");
ERROR:
		resp->version = 0x05;
		resp->reply = 0x01;
		close(conn_fd);
	}
	else
	{
		setnonblocking(conn_fd);
		
		resp->version = 0x05;
		resp->reply = 0x00;
		resp->reserved = 0x00;
		resp->addr_type = 0x01;
		
	
        srv_ev = (fd_event_t*)malloc(sizeof(fd_event_t));
        srv_ev->data = ev;
        srv_ev->ep = ev->ep;
        srv_ev->fd = conn_fd;
        srv_ev->ready_r = 0;
        srv_ev->ready_w = 0;
        srv_ev->status = STAT_PROCESS_DATA;
        srv_ev->rbuf = buffer_init(MAXN_BUFFER);
        srv_ev->wbuf = buffer_init(MAXN_BUFFER);
       	srv_ev->handle = handle_connection;
        ev->data = srv_ev;
        ev->status = STAT_PROCESS_DATA;

        epoll_ev.data.ptr = srv_ev;
        epoll_ev.events = EPOLLIN | EPOLLET;
        epoll_ctl(srv_ev->ep, EPOLL_CTL_ADD, srv_ev->fd, &epoll_ev);

        epoll_ev.data.ptr = ev;
        epoll_ev.events = EPOLLOUT | EPOLLET;
        epoll_ctl(ev->ep, EPOLL_CTL_MOD, ev->fd, &epoll_ev);
	}

    //copy data to write buffer
    buffer_push(wbuf, (void*)msg, 10);
}

void process_data(fd_event_t *ev)
{
    struct epoll_event epoll_ev;
    fd_event_t *opp_ev;
    buffer_t *rbuf;
    buffer_t *wbuf;

    opp_ev = (fd_event_t*)(ev->data);

    rbuf = ev->rbuf;
    wbuf = opp_ev->wbuf;

    buffer_push(wbuf, (void*)(rbuf->data + rbuf->first), BUFFER_SIZE(rbuf));
    buffer_clear(rbuf);

    epoll_ev.data.ptr = opp_ev;
    epoll_ev.events = EPOLLOUT | EPOLLET;
    epoll_ctl(opp_ev->ep, EPOLL_CTL_MOD, opp_ev->fd, &epoll_ev);
}

