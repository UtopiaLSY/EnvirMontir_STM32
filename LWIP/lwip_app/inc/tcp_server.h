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

#define     SERVER_PORT     8090                //服务器端口号
#define     TCP_SERVER_RX_BUFSIZE   100         //最大接收缓存区


//TCP服务器连接状态
enum tcp_server_states
{
	ES_TCPSERVER_NONE = 0,		//没有连接
	ES_TCPSERVER_ACCEPTED,		//有客户端连接上了 
	ES_TCPSERVER_CLOSING,		//即将关闭连接
};

struct _tcp_server_data
{
    unsigned char  state;       //当前连接状态
    struct tcp_pcb *pcb;        //指向当前的pcb
    struct pbuf *p;             //指向接收/发送的pbuf
};

extern u8 tcp_server_flag;
extern u8 tcp_server_revdata[TCP_SERVER_RX_BUFSIZE];

void tcp_echoserver_init(void);


#endif
