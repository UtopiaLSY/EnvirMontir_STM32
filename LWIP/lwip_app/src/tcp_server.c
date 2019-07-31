#include "tcp_server.h"


u8 tcp_server_revdata[TCP_SERVER_RX_BUFSIZE];
const u8 *tcp_server_sendbuf = "this server send test\r\n";


/* TCP Server ȫ��״̬��Ǳ���
    BIN(0 0 0 00000)
        | | | --------------����
        | | |---------------bit5,0:û�пͻ�������;1:�пͻ�������
        | |-----------------bit6,0:û���յ�����;1:�յ�����
        |-------------------bit7,0:û������Ҫ����;1:������Ҫ���� */
u8 tcp_server_flag = 0;	



//�ֲ���������
err_t tcp_accept_server (void *arg, struct tcp_pcb *newpcb, err_t err); //tcp_accept�ص�����
err_t tcp_recv_server (void *arg, struct tcp_pcb *tpcb,struct pbuf *p, err_t err);
err_t tcp_sent_server(void *arg, struct tcp_pcb *tpcb, u16_t len);
void  tcp_err_server(void *arg, err_t err);
err_t tcp_poll_server(void *arg, struct tcp_pcb *tpcb);
void tcp_server_senddata(struct tcp_pcb *tpcb, struct _tcp_server_data *es);     //��������ݷ��ͺ���
void tcp_server_connect_close(struct tcp_pcb *tpcb, struct _tcp_server_data *es);

/************************************
*����:����tcp�����
*˵��:
************************************/
void tcp_echoserver_init(void)
{
    err_t err;
    struct tcp_pcb *server_tcp_pcb = NULL;      //����˾��
    server_tcp_pcb = tcp_new();
    if(server_tcp_pcb)
    {
        err = tcp_bind(server_tcp_pcb, IP_ADDR_ANY, SERVER_PORT);
        if(err == ERR_OK)       //�󶨶˿ڳɹ�
        {
            server_tcp_pcb = tcp_listen(server_tcp_pcb);        //�����˿�
            tcp_accept(server_tcp_pcb, tcp_accept_server);
        }
        else            //�󶨶˿�ʧ��
            ASSERT_ERROR("tcp_bind");
    }
    else 
        ASSERT_ERROR("tcp_new");
}

/************************************
*����:tcp_accept�ص�����
*˵��:
************************************/
err_t tcp_accept_server (void *arg, struct tcp_pcb *newpcb, err_t err)
{
    struct _tcp_server_data *es;
    tcp_setprio(newpcb, TCP_PRIO_MIN);      //�������ȼ�
    es = (struct _tcp_server_data*)mem_malloc(sizeof(struct _tcp_server_data));
    if(es != NULL)      //�ڴ����ɹ�
    {
        es->state = ES_TCPSERVER_ACCEPTED;  	//���յ��ͻ�������
        es->pcb = newpcb;
        es->p = NULL;
        
        tcp_arg(newpcb, es);
        tcp_recv(newpcb, tcp_recv_server);      //ע��recv�ص�����
        tcp_sent(newpcb, tcp_sent_server);      //ע��sent�ص�����
        tcp_err(newpcb, tcp_err_server);        //ע��err�ص�����
        tcp_poll(newpcb,tcp_poll_server,1);	    //ע��poll�ص�����
        tcp_server_flag |= 0x20;
        netdev_inform.remoteip[0] = newpcb->remote_ip.addr & 0xff;
        netdev_inform.remoteip[1] = (newpcb->remote_ip.addr>>8)&0xff;
        netdev_inform.remoteip[2] = (newpcb->remote_ip.addr>>16)&0xff;
        netdev_inform.remoteip[3] = (newpcb->remote_ip.addr>>24)&0xff;
        return ERR_OK;
    }
    else
    {
        ASSERT_ERROR("mem_malloc");
    }
}


/************************************
*����:tcp_recv�ص�����
*˵��:
************************************/
err_t tcp_recv_server (void *arg, struct tcp_pcb *tpcb,struct pbuf *p, err_t err)
{
    err_t ret_err;
    u32 data_len = 0;
    struct _tcp_server_data *es;
    struct pbuf *temp = NULL;
    
    es = (struct _tcp_server_data*)arg;
    if(p==NULL)             //���յ������ݣ��ر�TCP����
    {
        es->state = ES_TCPSERVER_CLOSING;
        es->p = p;
        ret_err = ERR_OK;
    }
    else if(err != ERR_OK)        //�������ݲ�Ϊ�գ�������ĳ��ԭ��err!=ERR_OK
    {
        if(p) pbuf_free(p);
        ret_err = err;
    }
    else if(es->state==ES_TCPSERVER_ACCEPTED)   //���յ����ݴ�������״̬
    {
        memset(tcp_server_revdata, 0, sizeof(tcp_server_revdata));
        for(temp = p; temp != NULL; temp = p->next)     //����������pbuf����
        {
            /*�ж�Ҫ������TCP_SERVER_RX_BUFSIZE�е������Ƿ����TCP_SERVER_RX_BUFSIZE��ʣ��ռ䣬�������
            **�Ļ���ֻ����TCP_SERVER_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����*/
            if(temp->len > (TCP_SERVER_RX_BUFSIZE-data_len))
                memcpy(tcp_server_revdata+data_len, temp->payload, (TCP_SERVER_RX_BUFSIZE-data_len));
            else
                memcpy(tcp_server_revdata+data_len, temp->payload, temp->len);
            data_len+=temp->len;
            if(data_len > TCP_SERVER_RX_BUFSIZE) break;
        }
        tcp_server_flag |= 1<<6;        //��ǽ��յ�����
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);
        ret_err = ERR_OK;
    }
    else            //�������ر���
    {
        tcp_recved(tpcb, p->tot_len);
        es->p = NULL;
         pbuf_free(p);
        ret_err = ERR_OK;
    }
    return ret_err;
}

/************************************
*����:tcp_sent�ص�����
*˵��:
************************************/
err_t tcp_sent_server(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    struct _tcp_server_data *es;
    LWIP_UNUSED_ARG(len); 
    es = (struct _tcp_server_data*)arg;
    if(es->p) tcp_server_senddata(tpcb, es);
    return ERR_OK;
}

/************************************
*����:��������
*˵��:
************************************/
void tcp_server_senddata(struct tcp_pcb *tpcb, struct _tcp_server_data *es)
{
    struct pbuf *ptr;
    u16 len;
    err_t wr_err = ERR_OK;
    while((wr_err == ERR_OK) && es->p && (es->p->len <= tcp_sndbuf(tpcb)))
    {
        ptr = es->p;
        wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
        if(wr_err == ERR_OK)
        {
            len = ptr->len;
            es->p = ptr->next;
            if(es->p) pbuf_ref(es->p);
            pbuf_free(ptr);
            tcp_recved(tpcb, len);
        }
        else if(wr_err == ERR_MEM) es->p = ptr;
    }
}

/************************************
*����:tcp_err�ص�����
*˵��:
************************************/
void tcp_err_server(void *arg, err_t err)
{
	printf("tcp error:%x\r\n",(u32)arg);
	if(arg!=NULL)mem_free(arg);//�ͷ��ڴ�
}

/************************************
*����:tcp_poll�ص�����
*˵��:
************************************/
err_t tcp_poll_server(void *arg, struct tcp_pcb *tpcb)
{
    err_t ret_err;
	struct _tcp_server_data *es; 
	es = (struct _tcp_server_data *)arg; 
    
    LED0 = !LED0;
    if(es!=NULL)
    {
        if(tcp_server_flag & (1<<7))
        {
            es->p = pbuf_alloc(PBUF_TRANSPORT, strlen((char*)tcp_server_sendbuf), PBUF_POOL);
            if(es->p == NULL) 
            {
                LED0 = LED_ON;
                while(1);
            }
            pbuf_take(es->p, (char*)tcp_server_sendbuf, strlen((char*)tcp_server_sendbuf));
            tcp_server_senddata(tpcb, es);
            tcp_server_flag &= ~(1<<7);     //���־λ
            if(es->p != NULL) pbuf_free(es->p);
        }
        else if(es->state == ES_TCPSERVER_CLOSING)
        {
            tcp_server_connect_close(tpcb, es);
        }
        ret_err = ERR_OK;
    }
    else
    {
        tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
		ret_err=ERR_ABRT; 
    }
    return ret_err;
}

void tcp_server_connect_close(struct tcp_pcb *tpcb, struct _tcp_server_data *es)
{
    tcp_close(tpcb);
	tcp_arg(tpcb,NULL);
	tcp_sent(tpcb,NULL);
	tcp_recv(tpcb,NULL);
	tcp_err(tpcb,NULL);
	tcp_poll(tpcb,NULL,0);
	if(es)mem_free(es); 
	tcp_server_flag&=~(1<<5);       //������ӶϿ���
}

