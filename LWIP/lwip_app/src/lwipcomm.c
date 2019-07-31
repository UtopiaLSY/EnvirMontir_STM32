#include "lwipcomm.h"


extern u32 memp_get_memorysize(void);	//在memp.c里面定义
extern u8_t *memp_memory;				//在memp.c里面定义.
extern u8_t *ram_heap;					//在mem.c里面定义.

u32 TCPTimer = 0;
u32 ARPTimer = 0;
u32 lwip_localtime = 0;         //内核定时时间
struct netif DM9000_netif;      //全局的网络接口
_net_dev netdev_inform;         //网卡信息结构体
u8 LocalIPAddr[4] = {192, 168, 1, 30};


//局部函数
void lwip_comm_mem_free(void);
    


/************************************
*操作:TIM3定时器初始化函数
*说明:arr:自动重装载值，psc:预分频系数
************************************/
void TIM3_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
    
    TIM_TimeBaseStruct.TIM_Period = arr;
    TIM_TimeBaseStruct.TIM_Prescaler = psc;    
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);
    
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //使能指定的TIM3中断,允许更新中断
    
    NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x07;
    NVIC_Init(&NVIC_InitStruct);
    TIM_Cmd(TIM3, ENABLE);
}

/************************************
*操作:TIM3中断处理函数
*说明:用于LwIP的系统时钟,没20ms中断一次
************************************/
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		lwip_localtime += 10; //加10
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}

/************************************
*操作:获取当前时间，用于内核定时
*说明:
************************************/
unsigned int sys_now(void)
{
    return lwip_localtime;
}

/************************************
*操作:lwip中mem和memp的内存申请
*说明:返回值:0,成功;
    其他,失败
************************************/
u8 lwip_comm_mem_malloc(void)
{
    u32 mempsize;
    u32 ramheapsize;
    
    mempsize = memp_get_memorysize();
    memp_memory = mymalloc(INSRAM, mempsize);       //内部ram中为memp_memory申请内存
    ramheapsize = LWIP_MEM_ALIGN_SIZE(MEM_SIZE)+2*LWIP_MEM_ALIGN_SIZE(4*3)+MEM_ALIGNMENT;   //得到ram heap大小
    ram_heap=mymalloc(INSRAM,ramheapsize);	//为ram_heap申请内存 
    if(!memp_memory || !ram_heap)
    {
        lwip_comm_mem_free();
        return 1;
    }
    return 0;
}

/************************************
*操作:lwip中mem和memp的释放内存
*说明:
************************************/
void lwip_comm_mem_free(void)
{ 	
	myfree(INSRAM, memp_memory);
	myfree(INSRAM, ram_heap);
}

void LwIP_Init_task(void)
{
    struct ip_addr ipaddr, netmask, gw;
    
    if(lwip_comm_mem_malloc())
    {
        printf("lwip_comm_mem_malloc 失败\r\n");
        while(1);
    }
    TIM3_Init(999,719);     //定时器3频率为100hz,用于内核定时事件
    DM9000_Init();
    lwip_init();
    
    IP4_ADDR(&ipaddr, LocalIPAddr[0], LocalIPAddr[1], LocalIPAddr[2], LocalIPAddr[3]);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 1, 1);
    
    netif_add(&DM9000_netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, ethernet_input);
    netif_set_default(&DM9000_netif);
    netif_set_up(&DM9000_netif);
}


/************************************
*操作:读取数据包
*说明:当接收到数据后调用,从网络缓冲区中读取接收到的数据包并将其发送给LWIP处理 
************************************/
void lwip_pkt_handle(void)
{
    ethernetif_input(&DM9000_netif);
}
