#include "DM9000.h"
#include "SysTick.h"
#include "USART.h"

struct _DM9000Config DM9000Cfg;


/************************************
*����:DM9000_Init(void)
*˵��:��ʼ��DM9000
************************************/
u8 DM9000_Init(void)
{
    u32 temp;
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStruct;
	FSMC_NORSRAMTimingInitTypeDef ReadWriteTiming; 	//DM9000�Ķ�дʱ��

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG |\
                           RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);        //ʹ�ܸ��ù���ʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);          //ʹ��FSMCʱ��

    //PD7 �������
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStruct);

    //PG6 ��������
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOG, &GPIO_InitStruct);

    //PD0 1 4 5 8 9 10 14 15����
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|\
							   GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15; 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;        //�����������
    GPIO_Init(GPIOD, & GPIO_InitStruct);
    
    //PE7 8 9 10 11 12 13 14 15����
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|\
						       GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;        //�����������
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;		//PF13�������
    GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;		//PG9�������
    GPIO_Init(GPIOG, &GPIO_InitStruct);

    //�ⲿ�ж���6
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource6);
    EXTI_InitStruct.EXTI_Line = EXTI_Line6;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStruct);
    EXTI_ClearITPendingBit(EXTI_Line6);

    //�ж���6��NVIC�ж�����
    NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);

	ReadWriteTiming.FSMC_AddressSetupTime = 0;		//��ַ����ʱ��
	ReadWriteTiming.FSMC_AddressHoldTime = 0;
	ReadWriteTiming.FSMC_DataSetupTime = 3;		//���ݽ���ʱ��
	ReadWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	ReadWriteTiming.FSMC_CLKDivision = 0x00;
	ReadWriteTiming.FSMC_DataLatency = 0x00;
	ReadWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;//ʹ��ģʽA
	
	FSMC_NORSRAMInitStruct.FSMC_Bank = FSMC_Bank1_NORSRAM2;	//NE2
	FSMC_NORSRAMInitStruct.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStruct.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStruct.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStruct.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStruct.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStruct.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStruct.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStruct.FSMC_ReadWriteTimingStruct = &ReadWriteTiming;
	FSMC_NORSRAMInitStruct.FSMC_WriteTimingStruct = &ReadWriteTiming;
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStruct);
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2,ENABLE); //ʹ��FSMC��Bank1_Bank1_NORSRAM2
    
    temp=*(vu32*)(0x1FFFF7E8);				//��ȡSTM32��ΨһID��ǰ24λ��ΪMAC��ַ�����ֽ�
    DM9000Cfg.mode = DM9000_AUTO;
    DM9000Cfg.queue_packet_len = 0;
    DM9000Cfg.imr_all = IMR_PAR | IMR_PRI;

    DM9000Cfg.mac_addr[0] = 2;
    DM9000Cfg.mac_addr[1] = 0;
    DM9000Cfg.mac_addr[2] = 0;
    DM9000Cfg.mac_addr[3] = (temp>>16) & 0XFF;
    DM9000Cfg.mac_addr[4] = (temp>>8) & 0XFFF;
    DM9000Cfg.mac_addr[5] = temp & 0xFF;

    //��ʼ���鲥��ַ
	DM9000Cfg.multicase_addr[0]=0Xff;
	DM9000Cfg.multicase_addr[1]=0Xff;
	DM9000Cfg.multicase_addr[2]=0Xff;
	DM9000Cfg.multicase_addr[3]=0Xff;
	DM9000Cfg.multicase_addr[4]=0Xff;
	DM9000Cfg.multicase_addr[5]=0Xff;
	DM9000Cfg.multicase_addr[6]=0Xff;
	DM9000Cfg.multicase_addr[7]=0Xff; 
    
    DM9000_Reset();							//��λDM9000
	Delay_ms(100);
	temp=DM9000_GetDeiviceID();			//��ȡDM9000ID
	printf("DM9000 ID:%#x\r\n",temp);
	if(temp!=DM9000_ID) return 1; 			//��ȡID����
	DM9000_SetPHYMode(DM9000Cfg.mode);		//����PHY����ģʽ
	
	DM9000_WriteReg(DM9000_NCR,0X00);
	DM9000_WriteReg(DM9000_TCR,0X00);		//���Ϳ��ƼĴ�������
	DM9000_WriteReg(DM9000_BPTR,0X3F);	
	DM9000_WriteReg(DM9000_FCTR,0X38);
	DM9000_WriteReg(DM9000_FCR,0X00);
	DM9000_WriteReg(DM9000_SMCR,0X00);		//����ģʽ
	DM9000_WriteReg(DM9000_NSR,NSR_WAKEST|NSR_TX2END|NSR_TX1END);//�������״̬
	DM9000_WriteReg(DM9000_ISR,0X0F);		//����ж�״̬
	DM9000_WriteReg(DM9000_TCR2,0X80);		//�л�LED��mode1 	
	//����MAC��ַ���鲥��ַ
	DM9000_SetMACAddress(DM9000Cfg.mac_addr);		//����MAC��ַ
	DM9000_SetMulticast(DM9000Cfg.multicase_addr);	//�����鲥��ַ
	DM9000_WriteReg(DM9000_RCR,RCR_DIS_LONG|RCR_DIS_CRC|RCR_RXEN);
	DM9000_WriteReg(DM9000_IMR,IMR_PAR); 
	temp=DM9000_GetSpeedAndDuplex();		//��ȡDM9000�������ٶȺ�˫��״̬
	if(temp!=0XFF)							//���ӳɹ���ͨ��������ʾ�����ٶȺ�˫��״̬
	{
		printf("DM9000 Speed:%dMbps,Duplex:%s duplex mode\r\n",(temp&0x02)?10:100,(temp&0x01)?"Full":"Half");
	}else printf("DM9000 Establish Link Failed!\r\n");
	DM9000_WriteReg(DM9000_IMR,DM9000Cfg.imr_all);	//�����ж�
	return 0;
}

/************************************
*����:��ȡDM9000ָ���Ĵ�����ֵ
*˵��:reg:�Ĵ�����ַ 
     ����ֵ��DM9000ָ���Ĵ�����ֵ
************************************/
u16 DM9000_ReadReg(u16 reg)
{
	DM9000->REG=reg;
	return DM9000->DATA; 
}

/************************************
*����:��DM9000ָ���Ĵ�����д��ָ��ֵ
*˵��:reg:Ҫд��ļĴ��� 
      data:Ҫд���ֵ
************************************/
void DM9000_WriteReg(u16 reg,u16 data)
{
	DM9000->REG=reg;
	DM9000->DATA=data;
}

/************************************
*����:��ȡDM9000��PHY��ָ���Ĵ���
*˵��:reg:Ҫ����PHY�Ĵ��� 
     ����ֵ:��ȡ����PHY�Ĵ���ֵ
************************************/
u16 DM9000_PHY_ReadReg(u16 reg)
{
	u16 temp;
	DM9000_WriteReg(DM9000_EPAR,DM9000_PHY|reg);
	DM9000_WriteReg(DM9000_EPCR,0X0C);				//ѡ��PHY�����Ͷ�����
	Delay_ms(10);
	DM9000_WriteReg(DM9000_EPCR,0X00);				//���������
	temp=(DM9000_ReadReg(DM9000_EPDRH)<<8)|(DM9000_ReadReg(DM9000_EPDRL));
	return temp;
}

/************************************
*����:��DM9000��PHY�Ĵ���д��ָ��ֵ
*˵��:reg:PHY�Ĵ��� 
      data:Ҫд���ֵ
************************************/
void DM9000_PHY_WriteReg(u16 reg,u16 data)
{
	DM9000_WriteReg(DM9000_EPAR,DM9000_PHY|reg);
	DM9000_WriteReg(DM9000_EPDRL,(data&0xff));		//д����ֽ�
	DM9000_WriteReg(DM9000_EPDRH,((data>>8)&0xff));	//д����ֽ�
	DM9000_WriteReg(DM9000_EPCR,0X0A);				//ѡ��PHY,����д����
	Delay_ms(50);
	DM9000_WriteReg(DM9000_EPCR,0X00);				//���д����	
}


/************************************
*����:��ȡDM9000��оƬID
*˵��:����ֵ��DM9000��оƬIDֵ
************************************/
u32 DM9000_GetDeiviceID(void)
{
	u32 value;
	value = DM9000_ReadReg(DM9000_VIDL);
	value |= DM9000_ReadReg(DM9000_VIDH) << 8;
	value |= DM9000_ReadReg(DM9000_PIDL) << 16;
	value |= DM9000_ReadReg(DM9000_PIDH) << 24;
	return value;
}

/************************************
*����:��ȡDM9000�������ٶȺ�˫��ģʽ
*˵��:����ֵ��0,100M��˫��
			  1,100Mȫ˫��
			  2,10M��˫��
			  3,10Mȫ˫��
			  0XFF,����ʧ�ܣ�
************************************/
u8 DM9000_GetSpeedAndDuplex(void)
{
	u8 temp;
	u8 i=0;	
	if(DM9000Cfg.mode==DM9000_AUTO)					//����������Զ�Э��ģʽһ��Ҫ�ȴ�Э�����
	{
		while(!(DM9000_PHY_ReadReg(0X01)&0X0020))	//�ȴ��Զ�Э�����
		{
			Delay_ms(100);					
			i++;
			if(i>100)return 0XFF;					//�Զ�Э��ʧ��
		}	
	}else											//�Զ���ģʽ,һ��Ҫ�ȴ����ӳɹ�
	{
		while(!(DM9000_ReadReg(DM9000_NSR)&0X40))	//�ȴ����ӳɹ�
		{
			Delay_ms(100);					
			i++;
			if(i>100)return 0XFF;					//����ʧ��			
		}
	}
	temp =((DM9000_ReadReg(DM9000_NSR)>>6)&0X02);	//��ȡDM9000�������ٶ�
	temp|=((DM9000_ReadReg(DM9000_NCR)>>3)&0X01);	//��ȡDM9000��˫��״̬
	return temp;
}

/************************************
*����:����DM900��PHY����ģʽ
*˵��:mode:PHYģʽ
************************************/
void DM9000_SetPHYMode(u8 mode)
{
	u16 BMCR_Value,ANAR_Value;	
	switch(mode)
	{
		case DM9000_10MHD:		//10M��˫��
			BMCR_Value=0X0000;
			ANAR_Value=0X21;
			break;
		case DM9000_10MFD:		//10Mȫ˫��
			BMCR_Value=0X0100;
			ANAR_Value=0X41;
			break;
		case DM9000_100MHD:		//100M��˫��
			BMCR_Value=0X2000;
			ANAR_Value=0X81;
			break;
		case DM9000_100MFD:		//100Mȫ˫��
			BMCR_Value=0X2100;
			ANAR_Value=0X101;
			break;
		case DM9000_AUTO:		//�Զ�Э��ģʽ
			BMCR_Value=0X1000;
			ANAR_Value=0X01E1;
			break;		
	}
	DM9000_PHY_WriteReg(DM9000_PHY_BMCR,BMCR_Value);
	DM9000_PHY_WriteReg(DM9000_PHY_ANAR,ANAR_Value);
 	DM9000_WriteReg(DM9000_GPR,0X00);	//ʹ��PHY
}

/************************************
*����:����DM9000��MAC��ַ
*˵��:macaddr��ָ��mac��ַ
************************************/
void DM9000_SetMACAddress(u8 *macaddr)
{
	u8 i;
	for(i=0;i<6;i++)
	{
		DM9000_WriteReg(DM9000_PAR+i,macaddr[i]);
	}
}


/************************************
*����:�����鲥��ַ
*˵��:
************************************/
void DM9000_SetMulticast(u8 *multicastaddr)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		DM9000_WriteReg(DM9000_MAR+i,multicastaddr[i]);
	}
}

/************************************
*����:��λDM9000
*˵��:
************************************/

void DM9000_Reset(void)
{
	//��λDM9000,��λ����ο�<DM9000 Application Notes V1.22>�ֲ�29ҳ
	DM9000_RST = 0;								//DM9000Ӳ����λ
	Delay_ms(10);
	DM9000_RST = 1; 							//DM9000Ӳ����λ����
	Delay_ms(100);								//һ��Ҫ�������ʱ����DM9000׼��������
 	DM9000_WriteReg(DM9000_GPCR,0x01);			//��һ��:����GPCR�Ĵ���(0X1E)��bit0Ϊ1 
	DM9000_WriteReg(DM9000_GPR,0);				//�ڶ���:����GPR�Ĵ���(0X1F)��bit1Ϊ0��DM9000�ڲ���PHY�ϵ�
 	DM9000_WriteReg(DM9000_NCR,(0x02|NCR_RST));	//������:�����λDM9000 
	do 
	{
		Delay_ms(25); 	
	}while(DM9000_ReadReg(DM9000_NCR)&1);		//�ȴ�DM9000��λ���
	DM9000_WriteReg(DM9000_NCR,0);
	DM9000_WriteReg(DM9000_NCR,(0x02|NCR_RST));	//DM9000�ڶ�����λ
	do 
	{
		Delay_ms(25);	
	}while (DM9000_ReadReg(DM9000_NCR)&1);
} 

/************************************
*����:ͨ��DM9000�������ݰ�
*˵��:p:pbuf�ṹ��ָ��
************************************/
void DM9000_SendPacket(struct pbuf *p)
{
    struct pbuf *temp = NULL;
    u16 pbuf_index = 0;
    u8 word[2];
    u8 i = 0;
    
    DM9000_WriteReg(DM9000_IMR,IMR_PAR);		//�ر������ж� 
	DM9000->REG=DM9000_MWCMD;					//���ʹ������Ϳ��Խ�Ҫ���͵����ݰᵽDM9000 TX SRAM��	
    temp = p;
    while(temp)
    {
        if(pbuf_index < temp->len)      //����һ�������е���Ч����
        {
            word[i++] = ((u8_t *)temp->payload)[pbuf_index++];  //��DM9000��TX SRAM��д�����ݣ�һ��д�������ֽ�����
            if(i == 2)
            {
                DM9000->DATA = ((u16)word[1]<<8) | word[0];
                i = 0;
            }
        }
        else            //׼��������һ��������
        {
            temp = temp->next;
            pbuf_index= 0;
        }
    }
    	//����һ���ֽ�δд��TX SRAM
	if(i==1)DM9000->DATA=word[0];
	//��DM9000д�뷢�ͳ���
	DM9000_WriteReg(DM9000_TXPLL,p->tot_len&0XFF);
	DM9000_WriteReg(DM9000_TXPLH,(p->tot_len>>8)&0XFF);		//����Ҫ�������ݵ����ݳ���
	DM9000_WriteReg(DM9000_TCR,0X01);						//�������� 
	while((DM9000_ReadReg(DM9000_ISR)&0X02)==0);			//�ȴ�������� 
	DM9000_WriteReg(DM9000_ISR,0X02);						//�����������ж� 
 	DM9000_WriteReg(DM9000_IMR,DM9000Cfg.imr_all);			//DM9000���������ж�ʹ��	
}

/************************************
*����:ͨ��DM9000�������ݰ�
*˵��:����pbuf�ṹ��ָ��
************************************/
struct pbuf *DM9000_Receive_Packet(void)
{
    struct pbuf *p = NULL;
    struct pbuf *q = NULL;
    u32 rxbyte;
    vu16 rx_status, rx_length;
    u16 *data;
    u16 dummy;
    int len;
__error_retry:
    DM9000_ReadReg(DM9000_MRCMDX);					//�ٶ�
	rxbyte=(u8)DM9000->DATA;						//���еڶ��ζ�ȡ
    if(rxbyte)
    {
        if(rxbyte > 1)      //rxbyte����1�����յ������ݴ���
        {
            printf("dm9000 rx: rx error, stop device\r\n");
            DM9000_WriteReg(DM9000_RCR,0x00);
            DM9000_WriteReg(DM9000_ISR,0x80);		 
            return (struct pbuf*)p;
        }
        DM9000->REG=DM9000_MRCMD;
        rx_status=DM9000->DATA;
        rx_length=DM9000->DATA;
        p = pbuf_alloc(PBUF_RAW, rx_length, PBUF_POOL);     //p�����ڴ��
        if(p != NULL)           //����ɹ�
        {
            for(q = p; q != NULL; q = q->next)
            {
                data = (u16*)q->payload;
                len = q->len;
                while(len>0)
                {
                    *data = DM9000->DATA;
                    len-=2;
                    data++;
                }
            }
        }
        else
        {
            printf("pbuf�ڴ�����ʧ��:%d\r\n", rx_length);
            data=&dummy;
            len=rx_length;
            while(len)
            {
                *data=DM9000->DATA;
                len-=2;
            }
        }
        //����rx_status�жϽ��������Ƿ�������´���FIFO�����CRC����
		//���������������������κ�һ�����ֵĻ�����������֡��
		//��rx_lengthС��64���ߴ���������ݳ��ȵ�ʱ��Ҳ����������֡
		if((rx_status&0XBF00) || (rx_length < 0X40) || (rx_length > DM9000_PKT_MAX))
		{
			printf("rx_status:%#x\r\n",rx_status);
			if (rx_status & 0x100)printf("rx fifo error\r\n");
            if (rx_status & 0x200)printf("rx crc error\r\n");
            if (rx_status & 0x8000)printf("rx length error\r\n");
            if (rx_length>DM9000_PKT_MAX)
			{
				printf("rx length too big\r\n");
				DM9000_WriteReg(DM9000_NCR, NCR_RST); 	//��λDM9000
				Delay_ms(5);
			}
			if(p!=NULL)pbuf_free((struct pbuf*)p);		//�ͷ��ڴ�
			p=NULL;
			goto __error_retry;
		}
    }
	else
    {
        DM9000_WriteReg(DM9000_ISR,ISR_PTS);			//��������жϱ�־λ
        DM9000Cfg.imr_all=IMR_PAR|IMR_PRI;				//���½����ж� 
        DM9000_WriteReg(DM9000_IMR, DM9000Cfg.imr_all);
    } 
	return (struct pbuf*)p; 
}

/************************************
*����:DM9000�жϴ�����
*˵��:
************************************/
void DMA9000_ISRHandler(void)
{
	u16 int_status;
	u16 last_io; 
	last_io = DM9000->REG;
	int_status=DM9000_ReadReg(DM9000_ISR); 
	DM9000_WriteReg(DM9000_ISR,int_status);	//����жϱ�־λ��DM9000��ISR�Ĵ�����bit0~bit5д1����
	if(int_status & ISR_ROS)
    if(int_status & ISR_ROOS)
	if(int_status & ISR_PRS)		//�����ж�
	{  
 		//��������жϣ��û���������������
	} 
	if(int_status&ISR_PTS)			//�����ж�
	{ 
		//��������жϣ��û���������������
	}
	DM9000->REG=last_io;	
}

/************************************
*����:�ⲿ�ж���6���жϷ�����
*˵��:
************************************/
void EXTI9_5_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line6); //����ж���6�����־λ
	while(DM9000_INT == 0)
	{
		DMA9000_ISRHandler();
	}
}




