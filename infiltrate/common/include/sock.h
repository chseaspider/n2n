/*
Copyright 2020 chseasipder

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef __SOCK_H__
#define __SOCK_H__

#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#define poll(a, b, c) WSAPoll(a, b, c)
#define close(a) closesocket(a)
#else
#include <netdb.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#endif
#include <fcntl.h>
#include <string.h>


#include "c_type.h"

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif

#define GUESE_PORT_MAX 3

typedef struct sock_s
{
	int fd;
	int poll_i;
	__u32 uptime;		// ���һ�����հ�/����ʱ�� jiffies

	__u8* recv_buf;		// ���ջ���
	int recv_buf_len;	// ���ջ����ܴ�С
	int recv_len;		// ��ǰ�ѽ������ݴ�С

	char* send_buf;		// ���ͻ���
	int send_buf_len;	// ���ͻ����ܴ�С
	int send_len;		// ��ǰ���������ݴ�С
}sock_t;

static inline void poll_add_write(struct pollfd* _poll)
{
	_poll->events |= POLLOUT;
}

static inline void poll_del_write(struct pollfd* _poll)
{
	_poll->events &= (~POLLOUT);
}

static inline void poll_add_read(struct pollfd* _poll)
{
	_poll->events |= POLLIN;
}

static inline void poll_del_read(struct pollfd* _poll)
{
	_poll->events &= (~POLLIN);
}

static inline int set_sock_block(int fd)
{
	//�����׽�������
#ifdef WIN32
	unsigned long ul = 0;
	int ret = ioctlsocket(fd, FIONBIO, (unsigned long *)&ul);
#else
	int ret = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
#endif
	if(ret == -1)
	{
		perror("set sock block:");
	}

	return ret;
}

static inline int set_sock_nonblock(int fd)
{
	//�����׽��ַ�����
#ifdef WIN32
	unsigned long ul = 1;
	int ret = ioctlsocket(fd, FIONBIO, (unsigned long *)&ul);
#else
	int ret = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
#endif
	if(ret == -1)
	{
		perror("set sock nonblock:");
	}

	return ret;
}

static inline __u32 StrToIp(const char *str)
{
	union {
		struct in_addr ipaddr;
		__u32 ip;
	}addr;
	memset(&addr, 0, sizeof(addr));
#ifdef WIN32
	inet_pton(AF_INET, str, &addr.ipaddr);
#else
	inet_aton(str, &addr.ipaddr);
#endif
	return addr.ip;
}

static inline char* IpToStr(__u32 ip)
{
	struct in_addr ipaddr;
	memcpy(&ipaddr, &ip, sizeof(ipaddr));
	return inet_ntoa(ipaddr);
}

int sock_add_poll(struct pollfd* _poll, int max, sock_t* sock);
int sock_del_poll(struct pollfd* _poll, int max, sock_t* sock);
int create_udp(sock_t *sock, __u32 ip, __u16 port);
int udp_sock_recv(sock_t * sock, struct sockaddr_in * addr);
int udp_sock_send(sock_t * sock, void * data, int data_len, __u32 ip, __u16 port);
void set_sockaddr_in(struct sockaddr_in *addr, __u32 ip, __u16 port);
void close_sock(sock_t *sock);
__u32 get_default_local_ip(void);

#endif

