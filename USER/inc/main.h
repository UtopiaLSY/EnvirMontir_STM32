#ifndef __MAIN_H
#define __MAIN_H
#include <stdio.h>
#include <string.h>
#include "SysGPIO.h"
#include "USART.h"
#include "LED.h"
#include "KEY.h"
#include "SysTick.h"
#include "OLED.h"
#include "sram.h"
#include "malloc.h"
#include "lwipcomm.h"
#include "lwip/timers.h"
#include "tcp_server.h"
#include "tcp_client.h"
#include "EnvirData.h"
#include "MHZ14.h"
#include "DHT11.h"


void HardWare_Init(void);



#endif
