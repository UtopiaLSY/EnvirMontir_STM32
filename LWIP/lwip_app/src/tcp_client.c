#include "tcp_client.h"

/* TCP Client ȫ��״̬��Ǳ���
    BIN(0 0 0 0 0000)
        | | | | ------------����
        | | | |-------------bit4,���յ�start�ź�
        | | |---------------bit5,0:û������;1:������
        | |-----------------bit6,0:û���յ�����;1:�յ�����
        |-------------------bit7,0:û������Ҫ����;1:������Ҫ���� */
u8 tcp_client_flag = 0;	
u8 tcp_client_revdata[TCP_CLIENT_MAXRX_BUFFSIZE];
struct tcp_pcb *client_pcb = NULL;

//�ֲ�����
err_t tcp_connect_client(void *arg, struct tcp_pcb *tpcb, err_t err);
err_t tcp_recv_client(void *arg, struct tcp_pcb *tpcb,struct pbuf *p, err_t err);
void  tcp_err_client(void *arg, err_t err);
err_t tcp_poll_client(void *arg, struct tcp_pcb *tpcb);
void tcp_client_senddata(struct tcp_pcb *tpcb, struct _tcp_client_data *es);     //��������ݷ��ͺ���

extern const unsigned char ICONBMP[][32];


void tcp_echoclient_init()
{
    
    ip_addr_t server_addr;
    
    client_pcb = tcp_new();
    if(client_pcb)
    {

        IP4_ADDR(&server_addr, 192, 168, 1, 100);
        tcp_connect(client_pcb, &server_addr, CLIENT_PORT, tcp_connect_client);
    }
    else
        ASSERT_ERROR("tcp_new");
}


err_t tcp_connect_client(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    struct _tcp_client_data *es;
    err_t ret_err;
    es = (struct _tcp_client_data*)arg;
    if(err == ERR_OK)
    {
        OLED_ShowBMP(110,0,126,16,1,ICONBMP[CONNECT_ICON]);
        OLED_RefreshGram();         //��ʾ���ӱ�־
        es = (struct _tcp_client_data*)mem_malloc(sizeof(struct _tcp_client_data));
        if(es)
        {
            es->state = ES_TCPCLIENT_CONNECTED;
            es->pcb = tpcb;
            es->p = NULL;
            tcp_arg(tpcb, es);
            tcp_recv(tpcb, tcp_recv_client);
            tcp_poll(tpcb, tcp_poll_client, 1);
            tcp_sent(tpcb, tcp_sent_client);
            tcp_err(tpcb, tcp_err_client);
            tcp_client_flag |= 1<<5;
            ret_err = ERR_OK;
        }
    }
    else
    {
        tcp_client_connect_close(tpcb, 0);
        ret_err = ERR_MEM;
    }
    return ret_err;
}


void  tcp_err_client(void *arg, err_t err)
{
    printf("tcp error:%x\r\n",(u32)arg);
	if(arg!=NULL)mem_free(arg);//�ͷ��ڴ�
}


err_t tcp_recv_client(void *arg, struct tcp_pcb *tpcb,struct pbuf *p, err_t err)
{
    struct _tcp_client_data *es;
    struct pbuf *temp;
    err_t ret_err;
    u32 data_len = 0;
    
    es = (struct _tcp_client_data*)arg;
    if(p == NULL)       //���յ������ݣ��ر�����
    {
        es->state = ES_TCPCLIENT_CLOSING;
        es->p = p;
        ret_err = ERR_OK;
    }
    else if(err != ERR_OK)
    {
        if(p) pbuf_free(p);
        ret_err = err;
    }
    else if(es->state == ES_TCPCLIENT_CONNECTED)        //���յ�����
    {
        if(p != NULL)
        {
            memset(tcp_client_revdata, 0, sizeof(tcp_client_revdata));
            for(temp = p; temp != NULL; temp = temp->next)
            {
                if(temp->len > (TCP_CLIENT_MAXRX_BUFFSIZE - data_len))
                    memcpy(tcp_client_revdata, temp->payload, TCP_CLIENT_MAXRX_BUFFSIZE - data_len);
                else
                    memcpy(tcp_client_revdata, temp->payload, temp->len);
                data_len += temp->len;
                if(data_len > TCP_CLIENT_MAXRX_BUFFSIZE) break;
            }
            tcp_client_flag |= (1<<6);
            tcp_recved(tpcb, p->tot_len);
            pbuf_free(p);
            ret_err = ERR_OK;
        }
    }
    else        //���յ����ݣ������ӹر�
    {
        tcp_recved(tpcb, p->tot_len);
        es->p = NULL;
        pbuf_free(p);
        ret_err = ERR_OK;
    }
    return ret_err;
}
    
err_t tcp_sent_client(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    struct _tcp_client_data *es;
	LWIP_UNUSED_ARG(len);
	es=(struct _tcp_client_data*)arg;
	if(es->p) 
        tcp_client_senddata(tpcb,es);//��������
	return ERR_OK;
}

err_t tcp_poll_client(void *arg, struct tcp_pcb *tpcb)
{
    err_t ret_err;
    struct _tcp_client_data *es;
    es = (struct _tcp_client_data*)arg;
    LED0 = !LED0;
    if(es != NULL)
    {
        if(tcp_client_flag&(1<<7))
        {
            //�޲���
        }
        if(es->state == ES_TCPCLIENT_CLOSING)
        {
            tcp_client_connect_close(tpcb, es);
        }
        ret_err = ERR_OK;
    }
    else
    {
        tcp_abort(tpcb);
        ret_err = ERR_ABRT;
    }
    return ret_err;
}


void tcp_client_senddata(struct tcp_pcb *tpcb, struct _tcp_client_data *es)
{
    struct pbuf *sendptr;
    err_t wr_err = ERR_OK;
    while((wr_err == ERR_OK) && es->p && (es->p->len <= tcp_sndbuf(tpcb)))
    {
        sendptr = es->p;
        wr_err = tcp_write(tpcb, sendptr->payload, sendptr->len, 1);
        if(wr_err == ERR_OK)
        {
            es->p = sendptr->next;
            if(es->p)   pbuf_ref(es->p);
            pbuf_free(sendptr);
        }
        else if(wr_err == ERR_MEM) es->p = sendptr;
    }
}


void tcp_client_connect_close(struct tcp_pcb *tpcb, struct _tcp_client_data *es)
{
    //�Ƴ��ص�
    tcp_close(tpcb);
	tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
	tcp_arg(tpcb,NULL);  
	tcp_recv(tpcb,NULL);
	tcp_sent(tpcb,NULL);
	tcp_err(tpcb,NULL);
	tcp_poll(tpcb,NULL,0);
    OLED_ShowBMP(110,0,126,16,1,ICONBMP[DISCONNECT_ICON]);
    OLED_RefreshGram();         //��ʾ���ӶϿ���־
    LED0 = LED_OFF;
	if(es)mem_free(es); 
	tcp_client_flag = 0;        //��λ��־��
    client_pcb = NULL;
}

