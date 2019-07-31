#ifndef __SysTick_H
#define __SysTick_H
#include "stm32f10x.h"


void SysTick_Init(void);
void Delay_us(u32 us);
void Delay_ms(u32 ms);


#endif
