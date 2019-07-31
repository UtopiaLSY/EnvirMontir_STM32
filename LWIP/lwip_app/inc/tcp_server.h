#ifndef __TCP_SERVER_H
#define __TCP_SERVER_H
#include "lwip/tcp.h"
#include "stm32f10x.h"
#include "LED.h"
#include "SysTick.h"
#include "lwipcomm.h"
#include "OLED.h"
#include "debug_conf.h"
#include <string.h>

#define     SERVER_PORT     8090                //�������˿ں�
#define     TCP_SERVER_RX_BUFSIZE   100         //�����ջ�����


//TCP����������״̬
enum tcp_server_states
{
	ES_TCPSERVER_NONE = 0,		//û������
	ES_TCPSERVER_ACCEPTED,		//�пͻ����������� 
	ES_TCPSERVER_CLOSING,		//�����ر�����
};

struct _tcp_server_data
{
    unsigned char  state;       //��ǰ����״̬
    struct tcp_pcb *pcb;        //ָ��ǰ��pcb
    struct pbuf *p;             //ָ�����/���͵�pbuf
};

extern u8 tcp_server_flag;
extern u8 tcp_server_revdata[TCP_SERVER_RX_BUFSIZE];

void tcp_echoserver_init(void);


#endif
