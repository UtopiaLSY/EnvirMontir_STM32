#ifndef __SYS_H__
#define __SYS_H__
#include "stm32f10x.h"


//二进制转换
#define BIN(n)  LONGTOBIN(0x##n##L)
#define LONGTOBIN(n)    ( \
    (n>>21) & 0x80 | \
    (n>>18) & 0x40 | \
    (n>>15) & 0x20 | \
    (n>>12) & 0x10 | \
    (n>>9)  & 0x08 | \
    (n>>6)  & 0x04 | \
    (n>>3)  & 0x02 | \
    (n>>0)  & 0x01 | \
)

//实现位带操作
#define BITBAND(addr, bitnum)   ((addr & 0xF0000000)+0x2000000+((addr & 0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *(volatile unsigned long *)(addr)
#define BIT_ADDR(addr,bitnum)   MEM_ADDR(BITBAND(addr, bitnum))


#define GPIOA_IDR_ADDR  (GPIOA_BASE+8)
#define GPIOB_IDR_ADDR  (GPIOB_BASE+8)
#define GPIOC_IDR_ADDR  (GPIOC_BASE+8)
#define GPIOD_IDR_ADDR  (GPIOD_BASE+8)
#define GPIOE_IDR_ADDR  (GPIOE_BASE+8)
#define GPIOF_IDR_ADDR  (GPIOF_BASE+8)
#define GPIOG_IDR_ADDR  (GPIOG_BASE+8)

#define GPIOA_ODR_ADDR  (GPIOA_BASE+12)
#define GPIOB_ODR_ADDR  (GPIOB_BASE+12)
#define GPIOC_ODR_ADDR  (GPIOC_BASE+12)
#define GPIOD_ODR_ADDR  (GPIOD_BASE+12)
#define GPIOE_ODR_ADDR  (GPIOE_BASE+12)
#define GPIOF_ODR_ADDR  (GPIOF_BASE+12)
#define GPIOG_ODR_ADDR  (GPIOG_BASE+12)


#define PAIN(n)     BIT_ADDR(GPIOA_IDR_ADDR, n)
#define PAOUT(n)    BIT_ADDR(GPIOA_ODR_ADDR, n)

#define PBIN(n)     BIT_ADDR(GPIOB_IDR_ADDR, n)
#define PBOUT(n)    BIT_ADDR(GPIOB_ODR_ADDR, n)

#define PCIN(n)     BIT_ADDR(GPIOC_IDR_ADDR, n)
#define PCOUT(n)    BIT_ADDR(GPIOC_ODR_ADDR, n)

#define PDIN(n)     BIT_ADDR(GPIOD_IDR_ADDR, n)
#define PDOUT(n)    BIT_ADDR(GPIOD_ODR_ADDR, n)

#define PEIN(n)     BIT_ADDR(GPIOE_IDR_ADDR, n)
#define PEOUT(n)    BIT_ADDR(GPIOE_ODR_ADDR, n)

#define PFIN(n)     BIT_ADDR(GPIOF_IDR_ADDR, n)
#define PFOUT(n)    BIT_ADDR(GPIOF_ODR_ADDR, n)

#define PGIN(n)     BIT_ADDR(GPIOG_IDR_ADDR, n)
#define PGOUT(n)    BIT_ADDR(GPIOG_ODR_ADDR, n)

#endif
