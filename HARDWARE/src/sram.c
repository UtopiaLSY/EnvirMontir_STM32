#include "sram.h"

/************************************
*����:��ʼ���ⲿSRAM
*˵��:
************************************/
void FSMC_SRAM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStruct;
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG,ENABLE);
  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |\
                                GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |\
                               GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 |\
                               GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |GPIO_Pin_15;
    GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 |\
                               GPIO_Pin_10;
    GPIO_Init(GPIOG, &GPIO_InitStruct);

    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;
    readWriteTiming.FSMC_AddressHoldTime = 0x0;
    readWriteTiming.FSMC_AddressSetupTime = 0x00;
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x0;
    readWriteTiming.FSMC_CLKDivision = 0x0;
    readWriteTiming.FSMC_DataLatency = 0x0;
    readWriteTiming.FSMC_DataSetupTime = 0x03;

    FSMC_NORSRAMInitStruct.FSMC_Bank = FSMC_Bank1_NORSRAM3;//  ��������ʹ��NE3 ��Ҳ�Ͷ�ӦBTCR[4],[5]��
    FSMC_NORSRAMInitStruct.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; 
    FSMC_NORSRAMInitStruct.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
    FSMC_NORSRAMInitStruct.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//�洢�����ݿ��Ϊ16bit  
    FSMC_NORSRAMInitStruct.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
    FSMC_NORSRAMInitStruct.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStruct.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStruct.FSMC_WrapMode = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStruct.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStruct.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//�洢��дʹ�� 
    FSMC_NORSRAMInitStruct.FSMC_WaitSignal = FSMC_WaitSignal_Disable;  
    FSMC_NORSRAMInitStruct.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable; // ��дʹ����ͬ��ʱ��
    FSMC_NORSRAMInitStruct.FSMC_WriteBurst = FSMC_WriteBurst_Disable;  
    FSMC_NORSRAMInitStruct.FSMC_ReadWriteTimingStruct = &readWriteTiming;
    FSMC_NORSRAMInitStruct.FSMC_WriteTimingStruct = &readWriteTiming; //��дͬ��ʱ��
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStruct);  //��ʼ��FSMC����

   	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  // ʹ��BANK3	    
}

/************************************
*����:��ָ����ַ��ʼ,����д��n���ֽ�.
*˵��:pbuffer:�ֽڵ�ַ
      WriteAddr:д���ַ
      len:�ֽڳ���
************************************/
void FSMC_SRAM_WriteBuffer(unsigned char *pbuffer, u32 WriteAddr, u32 len)
{
    for(; len != 0; len--)
    {
        *(vu8*)(BANK1_SRAM3_ADDR+WriteAddr) = *pbuffer;
        pbuffer++;
        WriteAddr++;
    }
}

/************************************
*����:��ָ����ַ��ʼ,��������n���ֽ�.
*˵��:pbuffer:�ֽڵ�ַ
      ReadAddr:д���ַ
      len:�ֽڳ���
************************************/
void FSMC_SRAM_ReadBuffer(u8 *pbuffer, u32 ReadAddr, u32 len)
{
    while(len--)
    {
        *pbuffer = *(vu8*)(BANK1_SRAM3_ADDR+ReadAddr);
        pbuffer++;
        ReadAddr++;
    }
}

/************************************
*����:���ԣ���ָ����ַд��1���ֽ�����
*˵��:
************************************/
void FSMC_SRAM_WriteOneByte(u8 data, u32 addr)
{
    FSMC_SRAM_WriteBuffer(&data, addr, 1);
}

/************************************
*����:���ԣ���ָ����ַ��ȡ1���ֽ�����
*˵��:
************************************/
u8 FSMC_SRAM_ReadOneByte(u32 addr)
{
    u8 data;
    FSMC_SRAM_ReadBuffer(&data, addr, 1);
    return data;
}

