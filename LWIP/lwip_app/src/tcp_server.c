#include "tcp_server.h"


u8 tcp_server_revdata[TCP_SERVER_RX_BUFSIZE];
const u8 *tcp_server_sendbuf = "this server send test\r\n";


/* TCP Server 全局状态标记变量
    BIN(0 0 0 00000)
        | | | --------------保留
        | | |---------------bit5,0:没有客户端连接;1:有客户端连接
        | |-----------------bit6,0:没有收到数据;1:收到数据
        |-------------------bit7,0:没有数据要发送;1:有数据要发送 */
u8 tcp_server_flag = 0;	



//局部函数声明
err_t tcp_accept_server (void *arg, struct tcp_pcb *newpcb, err_t err); //tcp_accept回调函数
err_t tcp_recv_server (void *arg, struct tcp_pcb *tpcb,struct pbuf *p, err_t err);
err_t tcp_sent_server(void *arg, struct tcp_pcb *tpcb, u16_t len);
void  tcp_err_server(void *arg, err_t err);
err_t tcp_poll_server(void *arg, struct tcp_pcb *tpcb);
void tcp_server_senddata(struct tcp_pcb *tpcb, struct _tcp_server_data *es);     //服务端数据发送函数
void tcp_server_connect_close(struct tcp_pcb *tpcb, struct _tcp_server_data *es);

/************************************
*操作:建立tcp服务端
*说明:
************************************/
void tcp_echoserver_init(void)
{
    err_t err;
    struct tcp_pcb *server_tcp_pcb = NULL;      //服务端句柄
    server_tcp_pcb = tcp_new();
    if(server_tcp_pcb)
    {
        err = tcp_bind(server_tcp_pcb, IP_ADDR_ANY, SERVER_PORT);
        if(err == ERR_OK)       //绑定端口成功
        {
            server_tcp_pcb = tcp_listen(server_tcp_pcb);        //监听端口
            tcp_accept(server_tcp_pcb, tcp_accept_server);
        }
        else            //绑定端口失败
            ASSERT_ERROR("tcp_bind");
    }
    else 
        ASSERT_ERROR("tcp_new");
}

/************************************
*操作:tcp_accept回调函数
*说明:
************************************/
err_t tcp_accept_server (void *arg, struct tcp_pcb *newpcb, err_t err)
{
    struct _tcp_server_data *es;
    tcp_setprio(newpcb, TCP_PRIO_MIN);      //设置优先级
    es = (struct _tcp_server_data*)mem_malloc(sizeof(struct _tcp_server_data));
    if(es != NULL)      //内存分配成功
    {
        es->state = ES_TCPSERVER_ACCEPTED;  	//接收到客户端连接
        es->pcb = newpcb;
        es->p = NULL;
        
        tcp_arg(newpcb, es);
        tcp_recv(newpcb, tcp_recv_server);      //注册recv回调函数
        tcp_sent(newpcb, tcp_sent_server);      //注册sent回调函数
        tcp_err(newpcb, tcp_err_server);        //注册err回调函数
        tcp_poll(newpcb,tcp_poll_server,1);	    //注册poll回调函数
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
*操作:tcp_recv回调函数
*说明:
************************************/
err_t tcp_recv_server (void *arg, struct tcp_pcb *tpcb,struct pbuf *p, err_t err)
{
    err_t ret_err;
    u32 data_len = 0;
    struct _tcp_server_data *es;
    struct pbuf *temp = NULL;
    
    es = (struct _tcp_server_data*)arg;
    if(p==NULL)             //接收到空数据，关闭TCP连接
    {
        es->state = ES_TCPSERVER_CLOSING;
        es->p = p;
        ret_err = ERR_OK;
    }
    else if(err != ERR_OK)        //接收数据不为空，但由于某种原因err!=ERR_OK
    {
        if(p) pbuf_free(p);
        ret_err = err;
    }
    else if(es->state==ES_TCPSERVER_ACCEPTED)   //接收到数据处于连接状态
    {
        memset(tcp_server_revdata, 0, sizeof(tcp_server_revdata));
        for(temp = p; temp != NULL; temp = p->next)     //遍历完整个pbuf链表
        {
            /*判断要拷贝到TCP_SERVER_RX_BUFSIZE中的数据是否大于TCP_SERVER_RX_BUFSIZE的剩余空间，如果大于
            **的话就只拷贝TCP_SERVER_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据*/
            if(temp->len > (TCP_SERVER_RX_BUFSIZE-data_len))
                memcpy(tcp_server_revdata+data_len, temp->payload, (TCP_SERVER_RX_BUFSIZE-data_len));
            else
                memcpy(tcp_server_revdata+data_len, temp->payload, temp->len);
            data_len+=temp->len;
            if(data_len > TCP_SERVER_RX_BUFSIZE) break;
        }
        tcp_server_flag |= 1<<6;        //标记接收到数据
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);
        ret_err = ERR_OK;
    }
    else            //服务器关闭了
    {
        tcp_recved(tpcb, p->tot_len);
        es->p = NULL;
         pbuf_free(p);
        ret_err = ERR_OK;
    }
    return ret_err;
}

/************************************
*操作:tcp_sent回调函数
*说明:
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
*操作:发送数据
*说明:
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
*操作:tcp_err回调函数
*说明:
************************************/
void tcp_err_server(void *arg, err_t err)
{
	printf("tcp error:%x\r\n",(u32)arg);
	if(arg!=NULL)mem_free(arg);//释放内存
}

/************************************
*操作:tcp_poll回调函数
*说明:
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
            tcp_server_flag &= ~(1<<7);     //清标志位
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
        tcp_abort(tpcb);//终止连接,删除pcb控制块
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
	tcp_server_flag&=~(1<<5);       //标记连接断开了
}

