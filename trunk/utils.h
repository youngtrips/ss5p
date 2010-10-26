#ifndef __UTILS_H
#define __UTILS_H

void setnonblocking(int sock);

int modify_send_buf(int sock, int recv_buf_size);

int modify_recv_buf(int sock, int recv_buf_size);

#endif
