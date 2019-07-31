#ifndef __SRAM_H
#define __SRAM_H
#include "stm32f10x.h"



#define BANK1_SRAM3_ADDR    ((u32)(0x68000000))

void FSMC_SRAM_Init(void);
void FSMC_SRAM_WriteBuffer(unsigned char *pbuffer, u32 WriteAddr, u32 len);
void FSMC_SRAM_ReadBuffer(u8 *pbuffer, u32 ReadAddr, u32 len);
void FSMC_SRAM_WriteOneByte(u8 data, u32 addr);
u8 FSMC_SRAM_ReadOneByte(u32 addr);

#endif
