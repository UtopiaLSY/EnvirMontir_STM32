#include "lwipcomm.h"


extern u32 memp_get_memorysize(void);	//��memp.c���涨��
extern u8_t *memp_memory;				//��memp.c���涨��.
extern u8_t *ram_heap;					//��mem.c���涨��.

u32 TCPTimer = 0;
u32 ARPTimer = 0;
u32 lwip_localtime = 0;         //�ں˶�ʱʱ��
struct netif DM9000_netif;      //ȫ�ֵ�����ӿ�
_net_dev netdev_inform;         //������Ϣ�ṹ��
u8 LocalIPAddr[4] = {192, 168, 1, 30};


//�ֲ�����
void lwip_comm_mem_free(void);
    


/************************************
*����:TIM3��ʱ����ʼ������
*˵��:arr:�Զ���װ��ֵ��psc:Ԥ��Ƶϵ��
************************************/
void TIM3_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
    
    TIM_TimeBaseStruct.TIM_Period = arr;
    TIM_TimeBaseStruct.TIM_Prescaler = psc;    
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);
    
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //ʹ��ָ����TIM3�ж�,��������ж�
    
    NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x07;
    NVIC_Init(&NVIC_InitStruct);
    TIM_Cmd(TIM3, ENABLE);
}

/************************************
*����:TIM3�жϴ�����
*˵��:����LwIP��ϵͳʱ��,û20ms�ж�һ��
************************************/
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		lwip_localtime += 10; //��10
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}

/************************************
*����:��ȡ��ǰʱ�䣬�����ں˶�ʱ
*˵��:
************************************/
unsigned int sys_now(void)
{
    return lwip_localtime;
}

/************************************
*����:lwip��mem��memp���ڴ�����
*˵��:����ֵ:0,�ɹ�;
    ����,ʧ��
************************************/
u8 lwip_comm_mem_malloc(void)
{
    u32 mempsize;
    u32 ramheapsize;
    
    mempsize = memp_get_memorysize();
    memp_memory = mymalloc(INSRAM, mempsize);       //�ڲ�ram��Ϊmemp_memory�����ڴ�
    ramheapsize = LWIP_MEM_ALIGN_SIZE(MEM_SIZE)+2*LWIP_MEM_ALIGN_SIZE(4*3)+MEM_ALIGNMENT;   //�õ�ram heap��С
    ram_heap=mymalloc(INSRAM,ramheapsize);	//Ϊram_heap�����ڴ� 
    if(!memp_memory || !ram_heap)
    {
        lwip_comm_mem_free();
        return 1;
    }
    return 0;
}

/************************************
*����:lwip��mem��memp���ͷ��ڴ�
*˵��:
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
        printf("lwip_comm_mem_malloc ʧ��\r\n");
        while(1);
    }
    TIM3_Init(999,719);     //��ʱ��3Ƶ��Ϊ100hz,�����ں˶�ʱ�¼�
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
*����:��ȡ���ݰ�
*˵��:�����յ����ݺ����,�����绺�����ж�ȡ���յ������ݰ������䷢�͸�LWIP���� 
************************************/
void lwip_pkt_handle(void)
{
    ethernetif_input(&DM9000_netif);
}
