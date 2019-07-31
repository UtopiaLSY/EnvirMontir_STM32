#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"
#include "SysGPIO.h"
#include "LED.h"
#include "SysTick.h"
#include "tcp_client.h"
#include "OLED.h"

//�����жϴ�������
#ifndef KEY_INTERRUPT
    #define KEY_INTERRUPT   1
#endif

#if KEY_INTERRUPT == 1

#ifndef KEY0_INTERRUPT_CONFIG             
    #define KEY0_INTERRUPT_CONFIG    1    //����KEY0�ж����� Ĭ��ʹ���ж�
#endif
#ifndef KEY1_INTERRUPT_CONFIG             
    #define KEY1_INTERRUPT_CONFIG    1   //����KEY1�ж����� Ĭ��ʹ���ж�
#endif
#ifndef KEY2_INTERRUPT_CONFIG             
    #define KEY2_INTERRUPT_CONFIG    1   //����KEY2�ж����� Ĭ��ʹ���ж�
#endif
#ifndef WKUP_INTERRUPT_CONFIG             
    #define WKUP_INTERRUPT_CONFIG    1   //����WKUP�ж����� Ĭ��ʹ���ж�
#endif

#endif

#define KEY0    PEIN(4)
#define KEY1    PEIN(3)
#define KEY2    PEIN(2)
#define WK_UP   PAIN(0)

#define KEY0_PRESS      1
#define KEY1_PRESS      2
#define KEY2_PRESS      3
#define WKUP_PRESS      4

void KEY_Init(void);
u8 KEY_Scan(u8 mode);


#endif
