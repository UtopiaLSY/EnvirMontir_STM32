#include "MHZ14.h"



//局部函数
void USATR3_Init(u32 bound);


const u8 MHZ14_ReadComm[9] =    {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
const u8 MHZ14_ZERO[9] =        {0xFF ,0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};

u8 MHZ14_RecvMeasureData[9];
extern EnvirParam  Envir_collectedVlaue;


void USART3_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;      //PB11--RX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;            //PB10--TX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    USART_InitStruct.USART_BaudRate = bound;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART3, &USART_InitStruct);
    USART_Cmd(USART3, ENABLE);
    
    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);
    
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}


void MHZ14_Init(void)
{
    USART3_Init(9600);
}

void MHZ14_SendMeasureComm(void)
{
    u8 cnt;
    for(cnt = 0; cnt < 9; cnt++)
    {
        USART_SendData(USART3, MHZ14_ReadComm[cnt]);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET);
    }
}


void USART3_IRQHandler(void)
{
    static u8 revcnt = 0;
    u8 checksum=0;
    u8 i;
    
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        MHZ14_RecvMeasureData[revcnt++] = USART_ReceiveData(USART3);
        if(revcnt == 2)
        {
            if(MHZ14_RecvMeasureData[0] != 0xFF || MHZ14_RecvMeasureData[1] != 0x86)
            {
                revcnt = 0;     //接收数据有错，重新接收
                memset(MHZ14_RecvMeasureData, 0, sizeof(MHZ14_RecvMeasureData));
            }
        }
        if(revcnt == 9)     //完成一次接收测量值
        {
            revcnt = 0;
            for(i=1; i< 8; i++)
                checksum += MHZ14_RecvMeasureData[i];
            checksum = ~checksum;
            checksum+=1;
            if(checksum == MHZ14_RecvMeasureData[8])
            {
                checksum = 0;
                Envir_collectedVlaue.CO2 = MHZ14_RecvMeasureData[2]*256+MHZ14_RecvMeasureData[3];
                CO2Data_Ready = TRUE;
            }
        }
    }
}
