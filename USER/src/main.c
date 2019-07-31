#include "main.h"


//局部函数
void HardWare_Init(void);
void OLED_ShowInitInterface(void);
void OLED_ShowInterfaceData(u8 index);

u8 InterfaceIndex = 0;      //界面索引号


extern const unsigned char ICONBMP[][32];
extern u8 TempHumiData_Ready;

/*********************************
*操作:初始化底层硬件
*说明:
**********************************/
void HardWare_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
    SysTick_Init();
    LED_Init();
    KEY_Init();
    OLED_Init();
    USART1_Init(115200);
    FSMC_SRAM_Init();
    my_mem_init(INSRAM);
    my_mem_init(EXSRAM);
    LwIP_Init_task();
    MHZ14_Init();
    DHT11_Init();
    TIM2_Init(799,8999);        //定时周期100ms
}

/*********************************
*操作:初始化数据
*说明:
**********************************/
void Data_Init(void)
{
    PacketData_Init();
}


/*********************************
*操作:显示开机界面
*说明:
**********************************/
void OLED_ShowInitInterface(void)
{
    u8 i;
    for(i = 0; i < 6; i++)
    {
        OLED_ShowHZ1608(10+i*16, 0, 1, i);
    }
    
    OLED_ShowBMP(110,0,126,16,1,ICONBMP[DISCONNECT_ICON]);
    OLED_ShowString(0, 16, 1, 16, (u8*)"CO2:       ppm");
    OLED_ShowString(0, 32, 1, 16, (u8*)"Temp:      C ");
    OLED_ShowString(0, 48, 1, 16, (u8*)"Hum:       % ");
    OLED_RefreshGram();
}

/*********************************
*操作:根据界面号显示界面中的数据
*说明:index：界面索引号
**********************************/
void OLED_ShowInterfaceData(u8 index)
{
    unsigned char showremoteip[20];
    u8 showstart_flag[20];
    u32_t remoteaddr;
    u8 showtemp[8], showhumi[8];
    static u8 prestatus = 0xFF, preindex = 0xFF, prestart = 0xFF;        //记录之前的状态信息
    if( preindex != index || (prestart != ((tcp_client_flag>>START_FLAG)&0x01)) )
    {
        preindex = index;
        prestart = (tcp_client_flag>>START_FLAG)&0x01;
        prestatus = 0xFF;       //强制更新界面
    }
    
    switch(index)
    {
        case 0:
            if(CO2Data_Ready)
            {
                OLED_ShowNum(40,16,1,16,4,Envir_collectedVlaue.CO2);
                OLED_RefreshGram();
                CO2Data_Ready = FALSE;
            }
            if(TempHumiData_Ready)
            {
                sprintf((char*)showtemp, "%6.2f", Envir_collectedVlaue.temperature);
                sprintf((char*)showhumi, "%6.2f", Envir_collectedVlaue.humidity);
                OLED_ShowString(40, 32, 1, 16, showtemp);
                OLED_ShowString(40, 48, 1, 16, showhumi);
                OLED_RefreshGram();
                TempHumiData_Ready = FALSE;
            }
        break;
            
        case 1:
            if(prestatus != ((tcp_client_flag >> CONNECT_FLAG)&0x01))       //若与之前的连接状态不同，更新显示信息
            {
                prestatus = (tcp_client_flag >> CONNECT_FLAG)&0x01;            //记录连接状态
                if((tcp_client_flag & (1<<CONNECT_FLAG))==0)        //没有建立连接
                {
                    strcpy((char*)showremoteip, "NO Connect...       ");
                }
                else
                {
                    remoteaddr = client_pcb->remote_ip.addr;
                    sprintf((char*)showremoteip, "RMIP:%d.%d.%d.%d", (remoteaddr&0xFF), ((remoteaddr>>8)&0xFF), ((remoteaddr>>16)&0xFF), ((remoteaddr>>24)&0xFF));
                }
                if(tcp_client_flag&(1<<START_FLAG))
                {
                    strcpy((char*)showstart_flag, "EnvirData Uploading");
                }
                else
                {
                    strcpy((char*)showstart_flag, "NO Uploading       ");
                }
                OLED_ShowString(0, 32, 1, 12, (u8*)showremoteip);
                OLED_ShowString(0, 44, 1, 12, (u8*)showstart_flag);
                OLED_RefreshGram();
            }
        break;
    }
}


int main(void)
{
    HardWare_Init();
    Data_Init();
    OLED_ShowInitInterface();
    
    LED0 = 1;
    LED1 = 0;
    TIM_Cmd(TIM2, ENABLE);
    while(1)
    {
        
        OLED_ShowInterfaceData(InterfaceIndex);
        if((tcp_client_flag & (1<<SENDDATA_FLAG)) && (tcp_client_flag & (1<<START_FLAG)))
        {
            Send_EnvirPacketData();
            tcp_client_flag &= ~(1<<SENDDATA_FLAG);
        }
        
        
        if(tcp_client_flag&(1<<REVCDATA_FLAG))
        {
            if(strcmp((char*)tcp_client_revdata, "start") == 0)
            {
                tcp_client_flag |= (1<<START_FLAG);
            }
            tcp_client_flag &= ~(1<<REVCDATA_FLAG);
        }
        sys_check_timeouts();       //处理内核定时事件
        lwip_pkt_handle();
    }
}
