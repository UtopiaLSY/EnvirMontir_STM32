#ifndef __OLED_H
#define __OLED_H
#include "SysGPIO.h"
#include "SysTick.h"


/**********************************
//OLED模式设置采用4线串行模式(SPI)

接口定义：PD6:D0(SCL) SPI时钟线
         PG14:D1(SDA) SPI数据线
         PG15:RES 复位
         PC1:DC 数据/命令选择
         PC3:CS 片选信号
**********************************/


#define OLED_SCL_Set() GPIO_SetBits(GPIOD,GPIO_Pin_6)   //SCL至1
#define OLED_SCL_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_6) //SCL至0

#define OLED_SDA_Set() GPIO_SetBits(GPIOG,GPIO_Pin_14)   //SDA至1
#define OLED_SDA_Clr() GPIO_ResetBits(GPIOG,GPIO_Pin_14) //SDA至0

#define OLED_RES_Set() GPIO_SetBits(GPIOG,GPIO_Pin_15)   //RES至1
#define OLED_RES_Clr() GPIO_ResetBits(GPIOG,GPIO_Pin_15) //RES至0

#define OLED_DC_Set() GPIO_SetBits(GPIOC,GPIO_Pin_1)   //CS至1,写数据
#define OLED_DC_Clr() GPIO_ResetBits(GPIOC,GPIO_Pin_1) //CS至0,写命令

#define OLED_CS_Set() GPIO_SetBits(GPIOC,GPIO_Pin_3)   //DS至1
#define OLED_CS_Clr() GPIO_ResetBits(GPIOC,GPIO_Pin_3) //DS至0

#define OLED_CMD 0  //写命令
#define OLED_DATA 1 //写数据

void OLED_Init(void);
void OLED_WR_Byte(u8 dat,u8 cmd);
void OLED_RefreshGram(void);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Clear(void);
void OLED_DrawPiont(u8 x,u8 y,u8 mode);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode);
void OLED_ShowChar(u8 x,u8 y,u8 mode,u8 size,u8 chr);
void OLED_ShowString(u8 x,u8 y,u8 mode,u8 size,u8 *p);
void OLED_ShowNum(u8 x,u8 y,u8 mode,u8 size,u8 len,u32 num);
u32 MathPow(u8 x,u8 n);
void OLED_ShowHZ1608(u8 x,u8 y,u8 mode,u8 pos);
void OLED_ShowBMP(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode,const unsigned char *BMP);

#endif
