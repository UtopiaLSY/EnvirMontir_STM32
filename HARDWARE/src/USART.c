#include "USART.h"
#include <stdio.h>


u8 USART1_RX_BUFF[USART1RX_MAXLEN];     //接收缓存区
/************************************
USART1_RX_STA为接收标志位
最高位：接收完成
次高位：接收到0xOD
************************************/
u16 USART1_RX_STA = 0;

//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#pragma import(__use_no_semihosting)  
struct __FILE 
{ 
	int handle; 
}; 
FILE __stdout;

//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{
	x = x; 
} 
//重定义fputc函数,以支持printf函数
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	return ch;
}

/************************************
*操作:初始化串口
*说明:bound:波特率
************************************/
void USART1_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;         //PA10 -- RX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_InitStruct.USART_BaudRate = bound;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStruct);
    USART_Cmd(USART1, ENABLE);

    #if USART1_RX_ENABLE
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_Init(&NVIC_InitStruct);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);      //开启接收中断
    #endif
}




/************************************
*操作:串口接收中断
*说明:接收数据以0x0D 0x0A为结束
************************************/
void USART1_IRQHandler(void)
{
    u8 recvdata;

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        recvdata = USART_ReceiveData(USART1);
        if((USART1_RX_STA & 0x8000) == 0)        //接收未完成
        {
            if(USART1_RX_STA & 0x4000)           //已经接收到0x0D
            {
                if(recvdata == 0x0A)            //接收到0x0A,标记接收完成
                    USART1_RX_STA |= 0x8000;
                else USART1_RX_STA = 0;          //重新接收
            }
            else
            {
                if(recvdata == 0x0D)
                {
                    USART1_RX_STA |= 0x4000;
                }
                else 
                {
                    USART1_RX_BUFF[USART1_RX_STA&0x3FFF] = recvdata;
                    USART1_RX_STA++;
                    if((USART1RX_MAXLEN-1) < USART1_RX_STA) USART1_RX_STA = 0;      //接收数据超过指定最大长度
                }
            }
        }
    }
}


