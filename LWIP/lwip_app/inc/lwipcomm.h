#ifndef __LWIPCOMM_H
#define __LWIPCOMM_H
#include "stm32f10x.h"
#include "lwip/netif.h"
#include "lwip/init.h"
#include "netif/ethernetif.h"
#include "netif/etharp.h"
#include "DM9000.h"
#include "lwip/timers.h"
#include "lwip/tcp_impl.h"
#include "LED.h"
#include "malloc.h"


//Íø¿¨ÐÅÏ¢
typedef struct 
{
    u8 mac[6];
    u8 remoteip[4];
    u8 localip[4];
    u8 netmask[4];
    u8 gateway[4];
}_net_dev;

extern _net_dev netdev_inform;
extern u32 lwip_localtime;

void TIM3_Init(u16 arr, u16 psc);
void LwIP_Init_task(void);
void lwip_pkt_handle(void);
void lwip_periodic_handle(void);

#endif
