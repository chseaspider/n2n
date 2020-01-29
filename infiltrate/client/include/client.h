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

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "list.h"
#include "timer.h"
#include "sock.h"

#define INFP_DEFAFULT_PORT 45124 // TODO: �����ļ���ȡ

#define INFP_HASH_MAX 0x100
#define INFP_HASH_MASK 0xff

enum CLI_INFP_STATE
{
	CLI_INFP_INIT = 0,		// ��ʼ��
	CLI_INFP_LOGIN,			// �ѵ�½
};

typedef struct inf_proxy_s
{
	char  name[32];		// ��ʶ(MAC��ַ)
	struct sockaddr_in addr;// �Զ˹�����ַ
	int  fd;			// ������Զ�ͨ����fd
	__u32 uptime;		// ����յ��Է����ݰ�ʱ�� jiffies -> �ݶ�60�볬ʱ, ���ٸ����
	__u32 connected;	// �Ƿ�������

	struct list_head list_to; // ���� cli_infp_t->proxy_list
	struct hlist_node hash_to; // ���� cli_infp_t->proxy_hash
}inf_proxy_t;

/* ���һЩȫ�ֱ��� */
typedef struct cli_infp_s
{
	__u32 server_ip;		// ������ip (������)
	__u16 svr_m_port;		// ���������˿� (������)
	__u16 svr_b_port;		// ���������˿� (������)

	__u16 main_port;		// ����������ӵ����˿�, ��������
	__u16 proxy_port[3];	// ׼���Ĵ���˿�

	sock_t main_sock;		// ��Ӧmain_port
	sock_t proxy_sock[3];	// ��Ӧproxy_port (����������)

	__u32 nat_type;			// @see C_NAT_TYPE
	char  name[32];			// �ͻ��˱�ʶ (MAC��ַ)
	__u8  mode;				// �ͻ���ģʽ 0:host, 1:client
	__u8  pad[3];

	__u32 state;			// @see CLI_INFP_STATE
	__u32 next_login;		// �´ε�½
	__u32 next_hb;			// �´�����

	struct list_head proxy_list;	// NAT��͸�ĶԶ��豸���� -> inf_proxy_t
	struct hlist_head proxy_hash[INFP_HASH_MAX];// proxy_list��hash

	struct timer_list timer;	// 1��1�ε�timer (����timer����ͳһ������)
}cli_infp_t;

#define INFP_POLL_MAX 20		// ����д��

extern cli_infp_t gl_cli_infp;
extern struct pollfd poll_arr[];
extern int curfds;

#endif // __SERVER_H__
