#ifndef __DHT11_H
#define __DHT11_H
#include "stm32f10x.h"
#include "SysTick.h"
#include "SysGPIO.h"


//IO·½ÏòÉèÖÃ
#define DHT11_IO_IN()  {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=8<<12;}
#define DHT11_IO_OUT() {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=3<<12;}

#define DHT11_SDA_IN       PGIN(11)
#define DHT11_SDA_OUT      PGOUT(11)

void DHT11_Init(void);
u8 DHT11_ReadTempHum(float *temp, float *hum);


#endif
