#include "DHT11.h"


//�ֲ�����
void DHT11_Start(void);
u8 DHT11_WaiteCheck(void);
int DHT11_ReadBit(void);
u8 DHT11_ReadByte(void);

extern u8 TempHumiData_Ready;

void DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStruct);
    
    Delay_ms(2000);     //DHT11��ʼ���ȴ�2s
}

void DHT11_Start(void)
{
    DHT11_IO_OUT();
    DHT11_SDA_OUT = 0;
    Delay_ms(20);
    DHT11_SDA_OUT = 1;
    Delay_us(30);
}

/************************************
*����:�ȴ�DHT11����Ӧ
*˵��:����ֵ��1�����յ���Ӧ  0��δ���յ���Ӧ
************************************/
u8 DHT11_WaiteCheck(void)
{
    u8 delaytime = 0;
    
    DHT11_IO_IN();
    while(DHT11_SDA_IN && delaytime<=100)
    {
        delaytime++;
        Delay_us(1);
    }
    if(delaytime > 100)  return 1;       //������Ӧʧ��
    delaytime = 0;
    while(!DHT11_SDA_IN && delaytime<=100)
    {
        delaytime++;
        Delay_us(1);
    }
    if(delaytime > 100)  return 1;       //������Ӧʧ��
    else 
        return 0;       //������Ӧ�ɹ�
}

/************************************
*����:��ȡDHT11��λ����
*˵��:����ֵ��-1������λ���� 0/1������ֵ
************************************/
int DHT11_ReadBit(void)
{
    u8 delaytime = 0;
    while(DHT11_SDA_IN && delaytime<100)       //�ȴ���ɵ͵�ƽ
    {
        delaytime++;
        Delay_us(1);
    }
    delaytime = 0;
    while(!DHT11_SDA_IN && delaytime < 100)
    {
        delaytime++;
        Delay_us(1);
    }
    Delay_us(30);
    if(DHT11_SDA_IN)    return 1;
    else return 0;
}

u8 DHT11_ReadByte(void)
{
    u8 ret = 0;
    u8 cnt;
    for(cnt = 0; cnt < 8; cnt++)
    {
        ret <<=1;
        ret |= DHT11_ReadBit();
    }
    return ret;
}


u8 DHT11_ReadTempHum(float *temp, float *hum)
{
    u8 buf[5], i;
    DHT11_Start();
    if(DHT11_WaiteCheck() == 0)
    {
        for(i = 0; i < 5; i++)
            buf[i] = DHT11_ReadByte();
        if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
        {
            *temp = buf[0]+buf[1]*0.001;
            *hum = buf[2]+buf[3]*0.001;
            TempHumiData_Ready = 1;
        }
    }
    else return 1;
    return 0;
}



