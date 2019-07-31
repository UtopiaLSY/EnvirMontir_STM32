#include "key.h"


extern u8 InterfaceIndex;
extern u8  LocalIPAddr[4];


//局部函数
void OLED_ShowInterfaceFrame(u8 index);

/************************************
*操作:按键初始化
*说明:
************************************/
void KEY_Init(void)    
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    #if(KEY_INTERRUPT == 1)
        NVIC_InitTypeDef NVIC_InitStruct;
        EXTI_InitTypeDef EXTI_InitStruct;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
        EXTI_InitStruct.EXTI_LineCmd = ENABLE;
        EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x02;
    #endif
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE, ENABLE);
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_3 | GPIO_Pin_2;   //KEY0--PE4 KEY1--PE3 KEY2--PE2
    GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    #if KEY0_INTERRUPT_CONFIG
        GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);         //KEY0--PE4
        EXTI_InitStruct.EXTI_Line = EXTI_Line4;
        EXTI_Init(&EXTI_InitStruct);
        
        NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
        NVIC_Init(&NVIC_InitStruct);
     #endif
     
     #if KEY1_INTERRUPT_CONFIG
        GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);     //KEY1--PE3
        EXTI_InitStruct.EXTI_Line = EXTI_Line3;
        EXTI_Init(&EXTI_InitStruct);
        
        NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x02;
        NVIC_Init(&NVIC_InitStruct);
    #endif
        
    #if KEY2_INTERRUPT_CONFIG
        GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);     //KEY2--PE2
        EXTI_InitStruct.EXTI_Line = EXTI_Line2;
        EXTI_Init(&EXTI_InitStruct);
        
        NVIC_InitStruct.NVIC_IRQChannel = EXTI2_IRQn;
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x02;
        NVIC_Init(&NVIC_InitStruct);        
    #endif
    
    #if WKUP_INTERRUPT_CONFIG
        GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);     //WK_UP--PA0
        EXTI_InitStruct.EXTI_Line = EXTI_Line0;
        EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
        EXTI_Init(&EXTI_InitStruct);
        
        NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x03;
        NVIC_Init(&NVIC_InitStruct);
    #endif

}


#if (KEY_INTERRUPT == 0)
/************************************
*操作:扫描按键值
*说明:mode:0不支持连续按/1支持连续按
    返回按键值
************************************/
u8 KEY_Scan(u8 mode)
{
    static u8 keyflag = 1;      //按键松开标志
    if(mode)
        keyflag = 1;
    if(keyflag && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0 || WK_UP == 1))
    {
        Delay_ms(20);
        keyflag = 0;
        if(KEY0 == 0) return KEY0_PRESS;
        else if(KEY1 == 0) return KEY1_PRESS;
        else if(KEY2 == 0) return KEY2_PRESS;
        else if(WK_UP == 1) return WKUP_PRESS;
    }
    else if(KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && WK_UP == 0)
        keyflag = 1;
    return 0;
}

#else

#if WKUP_INTERRUPT_CONFIG
/************************************
*操作:按键WK_UP中断函数
*说明:建立客户端
************************************/
void EXTI0_IRQHandler(void)
{
    Delay_ms(30);
    if(WK_UP==1)
    {
        TIM_Cmd(TIM2, ENABLE);
        if(client_pcb == NULL)
        {
             tcp_echoclient_init();
        }
           
    }
    EXTI_ClearITPendingBit(EXTI_Line0);
}
#endif

#if KEY0_INTERRUPT_CONFIG
/************************************
*操作:按键KEY0中断函数
*说明:更新界面
************************************/
void EXTI4_IRQHandler(void)
{
    static u8 presscnt = 0;
    Delay_ms(30);
    if(KEY0==0)
    {
        presscnt++;
        if(presscnt == 1)
        {
            InterfaceIndex=0;
        }
        if(presscnt == 2)
        {
            InterfaceIndex=1;
            presscnt=0;
        }
        OLED_ShowInterfaceFrame(InterfaceIndex);
    }
    EXTI_ClearITPendingBit(EXTI_Line4);
}
#endif

#if KEY1_INTERRUPT_CONFIG
/************************************
*操作:按键KEY1中断函数
*说明:暂停上传数据
************************************/
void EXTI3_IRQHandler(void)
{
    Delay_ms(20);
    if(KEY1==1)
    {
        tcp_client_flag &= ~(1<<START_FLAG);        //清开始上传标志
    }
    EXTI_ClearITPendingBit(EXTI_Line3);
}
#endif

#if KEY2_INTERRUPT_CONFIG
/************************************
*操作:按键KEY2中断函数
*说明:
************************************/
void EXTI2_IRQHandler(void)
{
    Delay_ms(20);
    if(KEY2==1)
    {
        LED1 = !LED1;
    }
    EXTI_ClearITPendingBit(EXTI_Line2);
}
#endif

#endif

/************************************
*操作:根据界面号显示界面框架
*说明:index:界面号
************************************/
void OLED_ShowInterfaceFrame(u8 index)
{
    u8 showlocalip[22];
    
    OLED_Fill(0,16,127,63,0);
    switch(index)
    {
        case 0:
            OLED_ShowString(0, 16, 1, 16, (u8*)"CO2:       ppm");
            OLED_ShowString(0, 32, 1, 16, (u8*)"Temp:      C ");
            OLED_ShowString(0, 48, 1, 16, (u8*)"Hum:       % ");
            OLED_RefreshGram();
        break;
        
        case 1:
            sprintf((char*)showlocalip,"LocalIP:%d.%d.%d.%d", LocalIPAddr[0], LocalIPAddr[1], LocalIPAddr[2], LocalIPAddr[3]);
            OLED_ShowString(0, 20, 1, 12, (u8*)showlocalip);        //显示本地IP
            OLED_RefreshGram();
        break;
        
        default:break;
    }
}


