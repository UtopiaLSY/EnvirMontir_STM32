#ifndef __TCP_CLIENT_H
#define __TCP_CLIENT_H
#include "stm32f10x.h"
#include "lwip/tcp.h"
#include "debug_conf.h"
#include <string.h>
#include "LED.h"
#include "EnvirData.h"
#include "OLED.h"


#define     CLIENT_PORT                     9000
#define     TCP_CLIENT_MAXRX_BUFFSIZE       20
#define     START_FLAG                      4
#define     CONNECT_FLAG                    5
#define     REVCDATA_FLAG                   6
#define     SENDDATA_FLAG                   7


//LWIP回调函数使用的结构体
struct _tcp_client_data
{
	u8 state;               //当前连接状
	struct tcp_pcb *pcb;    //指向当前的pcb
	struct pbuf *p;         //指向接收/或传输的pbuf
};

//tcp服务器连接状态
enum tcp_client_states
{
	ES_TCPCLIENT_NONE = 0,		//没有连接
	ES_TCPCLIENT_CONNECTED,		//连接到服务器了 
	ES_TCPCLIENT_CLOSING,		//关闭连接
}; 

extern u8 tcp_client_flag;
extern u8 tcp_client_revdata[TCP_CLIENT_MAXRX_BUFFSIZE];
extern struct tcp_pcb *client_pcb;


void tcp_echoclient_init(void);
void tcp_client_connect_close(struct tcp_pcb *tpcb, struct _tcp_client_data *es);
void tcp_client_senddata(struct tcp_pcb *tpcb, struct _tcp_client_data *es);
err_t tcp_sent_client(void *arg, struct tcp_pcb *tpcb, u16_t len);

#endif
