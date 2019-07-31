#include "I2C.h"

//初始化IIC的信号引脚
void IIC_GPIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_SetBits(GPIOE, GPIO_Pin_0|GPIO_Pin_1);
}

//产生IIC的起始信号
void IIC_Start(void)
{
    IIC_SDAOut();
    IIC_SCL = 1;
    IIC_SDAWrite = 1;
    Delay_us(4);
    IIC_SDAWrite = 0;
    Delay_us(4);
    IIC_SCL = 0;
}

void IIC_Stop(void)
{
    IIC_SDAOut();
    IIC_SCL = 0;
    IIC_SDAWrite = 0;
    Delay_us(4);
    IIC_SCL = 1;
    IIC_SDAWrite= 1;
    Delay_us(4);
}

u8 IIC_WaitAck(void)
{
    u8 errtime = 0;
    
    IIC_SDAIn();
    IIC_SDAWrite = 1;
    Delay_us(1);
    IIC_SCL = 1;
    Delay_us(1);
    while(IIC_SDARead) {
        errtime++;
        if(errtime > 250) {
            IIC_Stop();
            return 1;           //接受应答失败
        }
    }
    IIC_SCL = 0;
    return 0;
}

void IIC_Ack(void)
{
    IIC_SDAOut();
    IIC_SCL = 0;
    Delay_us(2);
    IIC_SDAWrite = 0;
    IIC_SCL = 1;
    Delay_us(2);
    IIC_SCL = 0;
}

void IIC_NAck(void)
{
    IIC_SDAOut();
    IIC_SCL = 0;
    Delay_us(2);
    IIC_SDAWrite = 1;
    IIC_SCL = 1;
    Delay_us(2);
    IIC_SCL = 0;
}

void IIC_SendByte(u8 dat)
{
    u8 i;
    
    IIC_SDAOut();
    IIC_SCL = 0;
    for(i = 0; i < 8; i++) {
        IIC_SDAWrite = (dat & 0x80) >> 7;
        dat <<= 1;
        Delay_us(2);
        IIC_SCL = 1;
        Delay_us(2);
        IIC_SCL = 0;
        Delay_us(2);
    }
}

u8 IIC_ReadByte(unsigned char ack)
{
    u8 i;
    u8 dat = 0;
    
    IIC_SDAIn();
    IIC_SCL = 0;
    for(i = 0; i < 8; i++) {
        dat <<= 1;
        if(IIC_SDARead) {
            dat++;
        }
        Delay_us(4);
        IIC_SCL = 1;
        Delay_us(4);
        IIC_SCL = 0;
        Delay_us(4);
    }
    if(!ack)  IIC_NAck();
    else  IIC_Ack();
    
    return dat;
    
}
