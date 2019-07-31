#include "SysTick.h"
#include "stm32f10x.h"

static int usBaseCnt = 0;
static int msBaseCnt = 0;


void SysTick_Init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
    usBaseCnt = SystemCoreClock / 8000000;
    msBaseCnt = (u16)usBaseCnt * 1000;
}

void Delay_us(u32 us)
{
    u32 temp;
    SysTick->LOAD = us * usBaseCnt;
    SysTick->VAL = 0x0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do{
        temp = SysTick->CTRL;
    }while((temp & 0x01) && !(temp&(1<<16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x0;
}

void Delay_ms(u32 ms)
{
    u32 temp;
    SysTick->LOAD = ms * msBaseCnt;
    SysTick->VAL = 0x0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do{
        temp = SysTick->CTRL;
    }while((temp & 0x01) && !(temp&(1<<16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x0;
}
