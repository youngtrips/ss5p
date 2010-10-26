#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void setnonblocking(int sock)
{
    int opts;

	opts = fcntl(sock,F_GETFL);
    if(opts < 0)
    {
        perror("fcntl(sock,GETFL)");
        exit(1);
    }
    opts = opts|O_NONBLOCK;
    if(fcntl(sock,F_SETFL,opts)<0)
    {
        perror("fcntl(sock,SETFL,opts)");
        exit(1);
    }   
}

int modify_recv_buf(int sock, int recv_buf_size)
{
	return 0;
	return setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char*)&recv_buf_size, sizeof(int));
}

int modify_send_buf(int sock, int send_buf_size)
{
	return 0;
	return setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const char*)&send_buf_size, sizeof(int));
}
