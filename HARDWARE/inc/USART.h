#include "SysTick.h"
#ifndef __USART_H
#define __USART_H


#ifndef USART1_RX_ENABLE
#define USART1_RX_ENABLE    1
#endif

#define USART1RX_MAXLEN 200

extern u8  USART1_RX_BUFF[USART1RX_MAXLEN];
extern u16 USART1_RX_STA;               //每次接收完数据需清零

void USART1_Init(u32 bound);



#endif
