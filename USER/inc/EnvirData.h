#ifndef __EnvirData_H
#define __EnvirData_H
#include "stm32f10x.h"
#include "MHZ14.h"
#include "DHT11.h"
#include "debug_conf.h"
#include "tcp_client.h"


#define     indexCO2        0
#define     indexTemp       1
#define     indexHum        2

#define     DISCONNECT_ICON  0
#define     CONNECT_ICON     1


typedef struct _packetdata
{
    unsigned char name[10];
    unsigned char sendData[10];
}PacketData, *PPacketData;

typedef struct _envirData
{
    unsigned int CO2;
    float temperature;
    float humidity;
}EnvirParam;

extern PacketData Envir_Packetdata[3];
extern EnvirParam  Envir_collectedVlaue;
extern u8 CO2Data_Ready;

void PacketData_Init(void);
void TIM2_Init(u16 arr, u16 psc);
void Send_EnvirPacketData(void);


#endif
