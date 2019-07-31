#ifndef __LED_H
#define __LED_H
#include "SysGPIO.h"

#define LED0 PBOUT(5)
#define LED1 PEOUT(5)

#define LED_ON      0
#define LED_OFF     1

void LED_Init(void);


#endif
