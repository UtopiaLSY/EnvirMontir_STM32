#include "DM9000.h"
#include "SysTick.h"
#include "USART.h"

struct _DM9000Config DM9000Cfg;


/************************************
*操作:DM9000_Init(void)
*说明:初始化DM9000
************************************/
u8 DM9000_Init(void)
{
    u32 temp;
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStruct;
	FSMC_NORSRAMTimingInitTypeDef ReadWriteTiming; 	//DM9000的读写时序

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG |\
                           RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);        //使能复用功能时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);          //使能FSMC时钟

    //PD7 推挽输出
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStruct);

    //PG6 上拉输入
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOG, &GPIO_InitStruct);

    //PD0 1 4 5 8 9 10 14 15复用
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|\
							   GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15; 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;        //复用推挽输出
    GPIO_Init(GPIOD, & GPIO_InitStruct);
    
    //PE7 8 9 10 11 12 13 14 15复用
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|\
						       GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;        //复用推挽输出
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;		//PF13复用输出
    GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;		//PG9复用输出
    GPIO_Init(GPIOG, &GPIO_InitStruct);

    //外部中断线6
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource6);
    EXTI_InitStruct.EXTI_Line = EXTI_Line6;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStruct);
    EXTI_ClearITPendingBit(EXTI_Line6);

    //中断线6的NVIC中断配置
    NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);

	ReadWriteTiming.FSMC_AddressSetupTime = 0;		//地址建立时间
	ReadWriteTiming.FSMC_AddressHoldTime = 0;
	ReadWriteTiming.FSMC_DataSetupTime = 3;		//数据建立时间
	ReadWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	ReadWriteTiming.FSMC_CLKDivision = 0x00;
	ReadWriteTiming.FSMC_DataLatency = 0x00;
	ReadWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;//使用模式A
	
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
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2,ENABLE); //使能FSMC的Bank1_Bank1_NORSRAM2
    
    temp=*(vu32*)(0x1FFFF7E8);				//获取STM32的唯一ID的前24位作为MAC地址后三字节
    DM9000Cfg.mode = DM9000_AUTO;
    DM9000Cfg.queue_packet_len = 0;
    DM9000Cfg.imr_all = IMR_PAR | IMR_PRI;

    DM9000Cfg.mac_addr[0] = 2;
    DM9000Cfg.mac_addr[1] = 0;
    DM9000Cfg.mac_addr[2] = 0;
    DM9000Cfg.mac_addr[3] = (temp>>16) & 0XFF;
    DM9000Cfg.mac_addr[4] = (temp>>8) & 0XFFF;
    DM9000Cfg.mac_addr[5] = temp & 0xFF;

    //初始化组播地址
	DM9000Cfg.multicase_addr[0]=0Xff;
	DM9000Cfg.multicase_addr[1]=0Xff;
	DM9000Cfg.multicase_addr[2]=0Xff;
	DM9000Cfg.multicase_addr[3]=0Xff;
	DM9000Cfg.multicase_addr[4]=0Xff;
	DM9000Cfg.multicase_addr[5]=0Xff;
	DM9000Cfg.multicase_addr[6]=0Xff;
	DM9000Cfg.multicase_addr[7]=0Xff; 
    
    DM9000_Reset();							//复位DM9000
	Delay_ms(100);
	temp=DM9000_GetDeiviceID();			//获取DM9000ID
	printf("DM9000 ID:%#x\r\n",temp);
	if(temp!=DM9000_ID) return 1; 			//读取ID错误
	DM9000_SetPHYMode(DM9000Cfg.mode);		//设置PHY工作模式
	
	DM9000_WriteReg(DM9000_NCR,0X00);
	DM9000_WriteReg(DM9000_TCR,0X00);		//发送控制寄存器清零
	DM9000_WriteReg(DM9000_BPTR,0X3F);	
	DM9000_WriteReg(DM9000_FCTR,0X38);
	DM9000_WriteReg(DM9000_FCR,0X00);
	DM9000_WriteReg(DM9000_SMCR,0X00);		//特殊模式
	DM9000_WriteReg(DM9000_NSR,NSR_WAKEST|NSR_TX2END|NSR_TX1END);//清除发送状态
	DM9000_WriteReg(DM9000_ISR,0X0F);		//清除中断状态
	DM9000_WriteReg(DM9000_TCR2,0X80);		//切换LED到mode1 	
	//设置MAC地址和组播地址
	DM9000_SetMACAddress(DM9000Cfg.mac_addr);		//设置MAC地址
	DM9000_SetMulticast(DM9000Cfg.multicase_addr);	//设置组播地址
	DM9000_WriteReg(DM9000_RCR,RCR_DIS_LONG|RCR_DIS_CRC|RCR_RXEN);
	DM9000_WriteReg(DM9000_IMR,IMR_PAR); 
	temp=DM9000_GetSpeedAndDuplex();		//获取DM9000的连接速度和双工状态
	if(temp!=0XFF)							//连接成功，通过串口显示连接速度和双工状态
	{
		printf("DM9000 Speed:%dMbps,Duplex:%s duplex mode\r\n",(temp&0x02)?10:100,(temp&0x01)?"Full":"Half");
	}else printf("DM9000 Establish Link Failed!\r\n");
	DM9000_WriteReg(DM9000_IMR,DM9000Cfg.imr_all);	//设置中断
	return 0;
}

/************************************
*操作:读取DM9000指定寄存器的值
*说明:reg:寄存器地址 
     返回值：DM9000指定寄存器的值
************************************/
u16 DM9000_ReadReg(u16 reg)
{
	DM9000->REG=reg;
	return DM9000->DATA; 
}

/************************************
*操作:向DM9000指定寄存器中写入指定值
*说明:reg:要写入的寄存器 
      data:要写入的值
************************************/
void DM9000_WriteReg(u16 reg,u16 data)
{
	DM9000->REG=reg;
	DM9000->DATA=data;
}

/************************************
*操作:读取DM9000的PHY的指定寄存器
*说明:reg:要读的PHY寄存器 
     返回值:读取到的PHY寄存器值
************************************/
u16 DM9000_PHY_ReadReg(u16 reg)
{
	u16 temp;
	DM9000_WriteReg(DM9000_EPAR,DM9000_PHY|reg);
	DM9000_WriteReg(DM9000_EPCR,0X0C);				//选中PHY，发送读命令
	Delay_ms(10);
	DM9000_WriteReg(DM9000_EPCR,0X00);				//清除读命令
	temp=(DM9000_ReadReg(DM9000_EPDRH)<<8)|(DM9000_ReadReg(DM9000_EPDRL));
	return temp;
}

/************************************
*操作:向DM9000的PHY寄存器写入指定值
*说明:reg:PHY寄存器 
      data:要写入的值
************************************/
void DM9000_PHY_WriteReg(u16 reg,u16 data)
{
	DM9000_WriteReg(DM9000_EPAR,DM9000_PHY|reg);
	DM9000_WriteReg(DM9000_EPDRL,(data&0xff));		//写入低字节
	DM9000_WriteReg(DM9000_EPDRH,((data>>8)&0xff));	//写入高字节
	DM9000_WriteReg(DM9000_EPCR,0X0A);				//选中PHY,发送写命令
	Delay_ms(50);
	DM9000_WriteReg(DM9000_EPCR,0X00);				//清除写命令	
}


/************************************
*操作:获取DM9000的芯片ID
*说明:返回值：DM9000的芯片ID值
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
*操作:获取DM9000的连接速度和双工模式
*说明:返回值：0,100M半双工
			  1,100M全双工
			  2,10M半双工
			  3,10M全双工
			  0XFF,连接失败！
************************************/
u8 DM9000_GetSpeedAndDuplex(void)
{
	u8 temp;
	u8 i=0;	
	if(DM9000Cfg.mode==DM9000_AUTO)					//如果开启了自动协商模式一定要等待协商完成
	{
		while(!(DM9000_PHY_ReadReg(0X01)&0X0020))	//等待自动协商完成
		{
			Delay_ms(100);					
			i++;
			if(i>100)return 0XFF;					//自动协商失败
		}	
	}else											//自定义模式,一定要等待连接成功
	{
		while(!(DM9000_ReadReg(DM9000_NSR)&0X40))	//等待连接成功
		{
			Delay_ms(100);					
			i++;
			if(i>100)return 0XFF;					//连接失败			
		}
	}
	temp =((DM9000_ReadReg(DM9000_NSR)>>6)&0X02);	//获取DM9000的连接速度
	temp|=((DM9000_ReadReg(DM9000_NCR)>>3)&0X01);	//获取DM9000的双工状态
	return temp;
}

/************************************
*操作:设置DM900的PHY工作模式
*说明:mode:PHY模式
************************************/
void DM9000_SetPHYMode(u8 mode)
{
	u16 BMCR_Value,ANAR_Value;	
	switch(mode)
	{
		case DM9000_10MHD:		//10M半双工
			BMCR_Value=0X0000;
			ANAR_Value=0X21;
			break;
		case DM9000_10MFD:		//10M全双工
			BMCR_Value=0X0100;
			ANAR_Value=0X41;
			break;
		case DM9000_100MHD:		//100M半双工
			BMCR_Value=0X2000;
			ANAR_Value=0X81;
			break;
		case DM9000_100MFD:		//100M全双工
			BMCR_Value=0X2100;
			ANAR_Value=0X101;
			break;
		case DM9000_AUTO:		//自动协商模式
			BMCR_Value=0X1000;
			ANAR_Value=0X01E1;
			break;		
	}
	DM9000_PHY_WriteReg(DM9000_PHY_BMCR,BMCR_Value);
	DM9000_PHY_WriteReg(DM9000_PHY_ANAR,ANAR_Value);
 	DM9000_WriteReg(DM9000_GPR,0X00);	//使能PHY
}

/************************************
*操作:设置DM9000的MAC地址
*说明:macaddr：指向mac地址
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
*操作:设置组播地址
*说明:
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
*操作:复位DM9000
*说明:
************************************/

void DM9000_Reset(void)
{
	//复位DM9000,复位步骤参考<DM9000 Application Notes V1.22>手册29页
	DM9000_RST = 0;								//DM9000硬件复位
	Delay_ms(10);
	DM9000_RST = 1; 							//DM9000硬件复位结束
	Delay_ms(100);								//一定要有这个延时，让DM9000准备就绪！
 	DM9000_WriteReg(DM9000_GPCR,0x01);			//第一步:设置GPCR寄存器(0X1E)的bit0为1 
	DM9000_WriteReg(DM9000_GPR,0);				//第二步:设置GPR寄存器(0X1F)的bit1为0，DM9000内部的PHY上电
 	DM9000_WriteReg(DM9000_NCR,(0x02|NCR_RST));	//第三步:软件复位DM9000 
	do 
	{
		Delay_ms(25); 	
	}while(DM9000_ReadReg(DM9000_NCR)&1);		//等待DM9000软复位完成
	DM9000_WriteReg(DM9000_NCR,0);
	DM9000_WriteReg(DM9000_NCR,(0x02|NCR_RST));	//DM9000第二次软复位
	do 
	{
		Delay_ms(25);	
	}while (DM9000_ReadReg(DM9000_NCR)&1);
} 

/************************************
*操作:通过DM9000发送数据包
*说明:p:pbuf结构体指针
************************************/
void DM9000_SendPacket(struct pbuf *p)
{
    struct pbuf *temp = NULL;
    u16 pbuf_index = 0;
    u8 word[2];
    u8 i = 0;
    
    DM9000_WriteReg(DM9000_IMR,IMR_PAR);		//关闭网卡中断 
	DM9000->REG=DM9000_MWCMD;					//发送此命令后就可以将要发送的数据搬到DM9000 TX SRAM中	
    temp = p;
    while(temp)
    {
        if(pbuf_index < temp->len)      //拷贝一个数据中的有效数据
        {
            word[i++] = ((u8_t *)temp->payload)[pbuf_index++];  //向DM9000的TX SRAM中写入数据，一次写入两个字节数据
            if(i == 2)
            {
                DM9000->DATA = ((u16)word[1]<<8) | word[0];
                i = 0;
            }
        }
        else            //准备拷贝下一数据内容
        {
            temp = temp->next;
            pbuf_index= 0;
        }
    }
    	//还有一个字节未写入TX SRAM
	if(i==1)DM9000->DATA=word[0];
	//向DM9000写入发送长度
	DM9000_WriteReg(DM9000_TXPLL,p->tot_len&0XFF);
	DM9000_WriteReg(DM9000_TXPLH,(p->tot_len>>8)&0XFF);		//设置要发送数据的数据长度
	DM9000_WriteReg(DM9000_TCR,0X01);						//启动发送 
	while((DM9000_ReadReg(DM9000_ISR)&0X02)==0);			//等待发送完成 
	DM9000_WriteReg(DM9000_ISR,0X02);						//清除发送完成中断 
 	DM9000_WriteReg(DM9000_IMR,DM9000Cfg.imr_all);			//DM9000网卡接收中断使能	
}

/************************************
*操作:通过DM9000接收数据包
*说明:返回pbuf结构体指针
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
    DM9000_ReadReg(DM9000_MRCMDX);					//假读
	rxbyte=(u8)DM9000->DATA;						//进行第二次读取
    if(rxbyte)
    {
        if(rxbyte > 1)      //rxbyte大于1，接收到的数据错误
        {
            printf("dm9000 rx: rx error, stop device\r\n");
            DM9000_WriteReg(DM9000_RCR,0x00);
            DM9000_WriteReg(DM9000_ISR,0x80);		 
            return (struct pbuf*)p;
        }
        DM9000->REG=DM9000_MRCMD;
        rx_status=DM9000->DATA;
        rx_length=DM9000->DATA;
        p = pbuf_alloc(PBUF_RAW, rx_length, PBUF_POOL);     //p分配内存池
        if(p != NULL)           //分配成功
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
            printf("pbuf内存申请失败:%d\r\n", rx_length);
            data=&dummy;
            len=rx_length;
            while(len)
            {
                *data=DM9000->DATA;
                len-=2;
            }
        }
        //根据rx_status判断接收数据是否出现如下错误：FIFO溢出、CRC错误
		//对齐错误、物理层错误，如果有任何一个出现的话丢弃该数据帧，
		//当rx_length小于64或者大于最大数据长度的时候也丢弃该数据帧
		if((rx_status&0XBF00) || (rx_length < 0X40) || (rx_length > DM9000_PKT_MAX))
		{
			printf("rx_status:%#x\r\n",rx_status);
			if (rx_status & 0x100)printf("rx fifo error\r\n");
            if (rx_status & 0x200)printf("rx crc error\r\n");
            if (rx_status & 0x8000)printf("rx length error\r\n");
            if (rx_length>DM9000_PKT_MAX)
			{
				printf("rx length too big\r\n");
				DM9000_WriteReg(DM9000_NCR, NCR_RST); 	//复位DM9000
				Delay_ms(5);
			}
			if(p!=NULL)pbuf_free((struct pbuf*)p);		//释放内存
			p=NULL;
			goto __error_retry;
		}
    }
	else
    {
        DM9000_WriteReg(DM9000_ISR,ISR_PTS);			//清除所有中断标志位
        DM9000Cfg.imr_all=IMR_PAR|IMR_PRI;				//重新接收中断 
        DM9000_WriteReg(DM9000_IMR, DM9000Cfg.imr_all);
    } 
	return (struct pbuf*)p; 
}

/************************************
*操作:DM9000中断处理函数
*说明:
************************************/
void DMA9000_ISRHandler(void)
{
	u16 int_status;
	u16 last_io; 
	last_io = DM9000->REG;
	int_status=DM9000_ReadReg(DM9000_ISR); 
	DM9000_WriteReg(DM9000_ISR,int_status);	//清除中断标志位，DM9000的ISR寄存器的bit0~bit5写1清零
	if(int_status & ISR_ROS)
    if(int_status & ISR_ROOS)
	if(int_status & ISR_PRS)		//接收中断
	{  
 		//接收完成中断，用户自行添加所需代码
	} 
	if(int_status&ISR_PTS)			//发送中断
	{ 
		//发送完成中断，用户自行添加所需代码
	}
	DM9000->REG=last_io;	
}

/************************************
*操作:外部中断线6的中断服务函数
*说明:
************************************/
void EXTI9_5_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line6); //清除中断线6挂起标志位
	while(DM9000_INT == 0)
	{
		DMA9000_ISRHandler();
	}
}




