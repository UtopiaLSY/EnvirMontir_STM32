#include "EnvirData.h"
#include <string.h>
#include <stdio.h>


PacketData Envir_Packetdata[3] = {0};
EnvirParam  Envir_collectedVlaue;
u8 CO2Data_Ready = 0;
u8 TempHumiData_Ready = 0;


void TIM2_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    TIM_TimeBaseInitStruct.TIM_Period = arr;
    TIM_TimeBaseInitStruct.TIM_Prescaler = psc;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    TIM_Cmd(TIM2, DISABLE);
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //使能指定的TIM2中断,允许更新中断
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x06;
    NVIC_Init(&NVIC_InitStruct);
    
}

void PacketData_Init(void)
{
    Envir_collectedVlaue.CO2 = 0;
    Envir_collectedVlaue.humidity = 0;
    Envir_collectedVlaue.temperature = 0;
    memcpy(Envir_Packetdata[indexCO2].name, "CO2", 3);
    sprintf((char*)Envir_Packetdata[indexCO2].sendData, "%4d", Envir_collectedVlaue.CO2);
    memcpy(Envir_Packetdata[indexTemp].name, "Temperat", 8);
    sprintf((char*)Envir_Packetdata[indexTemp].sendData, "%6.2f", Envir_collectedVlaue.temperature);
    memcpy(Envir_Packetdata[indexHum].name, "Humidity", 8);
    sprintf((char*)Envir_Packetdata[indexHum].sendData, "%6.2f", Envir_collectedVlaue.humidity);
}

void Send_EnvirPacketData(void)
{
    struct _tcp_client_data *senddata;
    senddata = (struct _tcp_client_data*)mem_malloc(sizeof(struct _tcp_client_data));
    sprintf((char*)Envir_Packetdata[indexCO2].sendData, "%4d",Envir_collectedVlaue.CO2);
    sprintf((char*)Envir_Packetdata[indexTemp].sendData, "%6.2f",Envir_collectedVlaue.temperature);
    sprintf((char*)Envir_Packetdata[indexHum].sendData, "%6.2f",Envir_collectedVlaue.humidity);
    if(senddata != NULL)
    {
        senddata->p = pbuf_alloc(PBUF_TRANSPORT, sizeof(Envir_Packetdata),PBUF_POOL);
        senddata->p->payload = (char*)Envir_Packetdata;
        senddata->pcb = client_pcb;
        senddata->state = ES_TCPCLIENT_CONNECTED;
        tcp_sent(client_pcb, tcp_sent_client);
        tcp_client_senddata(client_pcb, senddata);
        if(senddata->p != NULL) pbuf_free(senddata->p);
        mem_free(senddata);
    }
    else
    {
        while(1);
    }
}


void TIM2_IRQHandler(void)
{
    static u8 timecnt = 0;
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断)
    {
        timecnt++;
        DHT11_ReadTempHum(&Envir_collectedVlaue.temperature, &Envir_collectedVlaue.humidity);
        if((timecnt%4) == 0)        //每400ms采集一次CO2
        {
            MHZ14_SendMeasureComm();
            LED1 = !LED1;           //运行指示灯
        }

        if((timecnt%10) == 0)       //每1s上传此次数据
        {
            tcp_client_flag |= (1<<SENDDATA_FLAG);
            timecnt = 0;
        }
    }
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}
