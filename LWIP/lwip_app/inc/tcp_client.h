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


//LWIP�ص�����ʹ�õĽṹ��
struct _tcp_client_data
{
	u8 state;               //��ǰ����״
	struct tcp_pcb *pcb;    //ָ��ǰ��pcb
	struct pbuf *p;         //ָ�����/�����pbuf
};

//tcp����������״̬
enum tcp_client_states
{
	ES_TCPCLIENT_NONE = 0,		//û������
	ES_TCPCLIENT_CONNECTED,		//���ӵ��������� 
	ES_TCPCLIENT_CLOSING,		//�ر�����
}; 

extern u8 tcp_client_flag;
extern u8 tcp_client_revdata[TCP_CLIENT_MAXRX_BUFFSIZE];
extern struct tcp_pcb *client_pcb;


void tcp_echoclient_init(void);
void tcp_client_connect_close(struct tcp_pcb *tpcb, struct _tcp_client_data *es);
void tcp_client_senddata(struct tcp_pcb *tpcb, struct _tcp_client_data *es);
err_t tcp_sent_client(void *arg, struct tcp_pcb *tpcb, u16_t len);

#endif
