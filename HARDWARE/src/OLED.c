#include "OLED.h"
#include "font.h"


//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127
u8 OLED_GRAM[128][8];


//初始化OLED
void OLED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG,ENABLE);
    
    //初始化 PC1\4 推挽输出
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
    
    //初始化 PD6 推挽输出
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
    GPIO_Init(GPIOD,&GPIO_InitStructure);
    
    //初始化 PG14\15 推挽输出
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14|GPIO_Pin_15;
    GPIO_Init(GPIOG,&GPIO_InitStructure);
    
    GPIO_SetBits(GPIOC,GPIO_Pin_1|GPIO_Pin_3);//输出高电平
    GPIO_SetBits(GPIOD,GPIO_Pin_6);
    GPIO_SetBits(GPIOG,GPIO_Pin_14|GPIO_Pin_15);
    
    OLED_RES_Clr();//OLED执行一次复位
    Delay_ms(100);
    OLED_RES_Set();
    Delay_ms(100);
    
    OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示
    OLED_WR_Byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
    OLED_WR_Byte(80,OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
    OLED_WR_Byte(0xA8,OLED_CMD); //设置驱动路数
    OLED_WR_Byte(0X3F,OLED_CMD); //默认0X3F(1/64) 
    OLED_WR_Byte(0xD3,OLED_CMD); //设置显示偏移
    OLED_WR_Byte(0X00,OLED_CMD); //默认为0

    OLED_WR_Byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.

    OLED_WR_Byte(0x8D,OLED_CMD); //电荷泵设置
    OLED_WR_Byte(0x14,OLED_CMD); //bit2，开启/关闭
    OLED_WR_Byte(0x20,OLED_CMD); //设置内存地址模式
    OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
    OLED_WR_Byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
    OLED_WR_Byte(0xC0,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
    OLED_WR_Byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
    OLED_WR_Byte(0x12,OLED_CMD); //[5:4]配置

    OLED_WR_Byte(0x81,OLED_CMD); //对比度设置
    OLED_WR_Byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
    OLED_WR_Byte(0xD9,OLED_CMD); //设置预充电周期
    OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
    OLED_WR_Byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
    OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

    OLED_WR_Byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
    OLED_WR_Byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示
    OLED_WR_Byte(0xAF,OLED_CMD); //开启显示

    OLED_Clear();

}

/*********************************************
            向SSD1106写入一个字节
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
*********************************************/
void OLED_WR_Byte(u8 dat,u8 cmd)
{
    u8 i;
    
    if(cmd)
        OLED_DC_Set();  //写数据
    else 
        OLED_DC_Clr();  //写命令
    OLED_CS_Clr();      //CS拉低
    for(i=0;i<8;i++) //从最高位发送8位数据
    {
        OLED_SCL_Clr();
        
        if(dat&0x80)  OLED_SDA_Set();
        else          OLED_SDA_Clr();
        
        OLED_SCL_Set();//上升沿发送数据
        dat<<=1;
    }
    OLED_CS_Set();
    OLED_DC_Set();
}

//更新显存到OLCD
void OLED_RefreshGram(void)
{
    u8 i,n;
    
    for(i=0;i<8;i++)
    {
        OLED_WR_Byte(0xB0+i,OLED_CMD);  //设置页地址
        OLED_WR_Byte(0x00,OLED_CMD);    //设置列地址低4位
        OLED_WR_Byte(0x10,OLED_CMD);    //设置列地址高4位
        
        for(n=0;n<128;n++)
        {
            OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA);//写数据
        }
    }
}

//开启OLED显示
void OLED_Display_On(void)
{
    OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵设置
    OLED_WR_Byte(0x14,OLED_CMD);//使能电荷泵
    OLED_WR_Byte(0xAF,OLED_CMD);//开显示
}

//关闭OLED显示
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵设置
    OLED_WR_Byte(0x10,OLED_CMD);//禁用电荷泵
    OLED_WR_Byte(0xAF,OLED_CMD);//关显示
}

//OLED清屏
void OLED_Clear(void)
{
    u8 x,y;
    
    for(x=0;x<128;x++)
    {
        for(y=0;y<8;y++)
        OLED_GRAM[x][y]=0x00;
    }
    OLED_RefreshGram();//更新显存到OLCD
}

/**************************
         画点函数
x:列,范围0-127
y:行,范围0-63
mode:1填充正常显示；0清空反白显示
***************************/
void OLED_DrawPiont(u8 x,u8 y,u8 mode)
{
    u8 xdat;//列的高低位8位数据
    u8 page;//页地址
    
    if(x>127 || y>63) return;//超出显示范围128*64，直接返回主函数

    page=7-y/8;           //设置页地址
    xdat=1<<(7-(y%8));    //设置列数据
    if(mode)  OLED_GRAM[x][page]|=xdat;//正常显示
    else      OLED_GRAM[x][page]&=~xdat;//反白显示
}

/*********************************************
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63
//mode:0,清空;1,填充
*********************************************/
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode)
{
    u8 xt,yt;
    
    if(x1>x2 || y1>y2)  return;//规划区域有问题，直接返回主函数
    
    for(xt=x1;xt<=x2;xt++)
    {
        for(yt=y1;yt<=y2;yt++)
        OLED_DrawPiont(xt,yt,mode);
    }
    OLED_RefreshGram();//更新显存到OLCD
}

/*********************************************
        在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示
//size:选择字体 12/16/24
//chr:显示的字符
*********************************************/
void OLED_ShowChar(u8 x,u8 y,u8 mode,u8 size,u8 chr)
{
    u8 temp,t,t1;
    u8 ysize=y;
    u8 csize=(size/8+((size%8)?1:0))*(size/2);      //得到字体一个字符对应点阵集所占的字节数
    chr=chr-' ';                                    //得到偏移后的值
    for(t=0;t<csize;t++)                           //每页从下至上扫描(顺向),逐列式
    {
        if(size==12) temp=asc2_1206[chr][t];            //根据字体大小选择相应ASICII字符集点阵
        else if(size==16) temp=asc2_1608[chr][t];
        else if(size==24) temp=asc2_2412[chr][t];
        else return;                                    //没有所要的字体大小，直接返回主函数
        for(t1=0;t1<8;t1++)
        {
            if(temp&0x80)  OLED_DrawPiont(x,y,mode);
            else           OLED_DrawPiont(x,y,!mode);
            temp<<=1;
            y++;
            if((y-ysize)==size)     //字符的一列显示完成，准备跳转到下一列显示
            {
                y=ysize;
                x++;                //跳转到下一列
                break;              //跳出for循环
            }
        }
    }
}

/*********************************************
                显示字符串
x,y:起点坐标  x:0~127  y:0~63
size:字体大小 12/16/24
(*p):字符串起始地址
*********************************************/
void OLED_ShowString(u8 x,u8 y,u8 mode,u8 size,u8 *p)
{
    while((*p>=' ') && (*p<='~'))   //判断是否符合合法字符
    {
        if(x>(128-(size/2)))    //若超出128列，则跳转到下一页显示
        {
            y+=size;
            x=0;
        }
        if(y>64-size)           //若超出(64-size)行，则清屏不显示
        {
            x=0;
            y=0;
            OLED_Clear();
        }
        OLED_ShowChar(x,y,mode,size,*p);
        x+=size/2;
        p++;    //地址加1
    }
}

//指数运算x^n
u32 MathPow(u8 x,u8 n)
{
    u32 result=1;
    while(n--)  result=result*x;
    return result;
}
/*******************************************************
                        显示数字
//x,y:起点坐标
//len:数字的位数
//size:字体大小
//mode:模式:0,填充模式;1,叠加模式
//num:数值(0~4294967295);
*******************************************************/
void OLED_ShowNum(u8 x,u8 y,u8 mode,u8 size,u8 len,u32 num)
{
    u8 tempNum,t;
    u8 showflag=0;
    
    for(t=0;t<len;t++)
    {
        tempNum=((num/MathPow(10,len-t-1))%10);//得到每一位的数值
        if(showflag==0 && t<(len-1))      //消去高位0的显示
        {
            if(tempNum==0)
            {
                OLED_ShowChar(x+((size/2)*t),y,mode,size,' ');
                continue;
            }
            else showflag=1;
        }
        OLED_ShowChar(x+((size/2)*t),y,mode,size,tempNum+'0');
    }
}

/*******************************************************
                        显示16*16的汉字
//x,y:起点坐标
//mode:模式:0,填充模式;1,叠加模式
//pos:汉字在数组中的位置
*******************************************************/
void OLED_ShowHZ1608(u8 x,u8 y,u8 mode,u8 pos)
{
    u8 t,i;
    u8 ysize=y;
    u8 temp=0;
    
    for(t=0;t<32;t++)       //每页从下至上(顺向),逐列式扫描
    {
        temp=HZ1608[pos][t];
        for(i=0;i<8;i++)
        {
            if(temp&0x80)   OLED_DrawPiont(x,y,mode);
            else            OLED_DrawPiont(x,y,!mode);
            temp<<=1;       //右移一位
            y++;
            if((y-ysize)==16)
            {
                y=ysize;
                x++;                //跳转到下一列
                break;              //跳出for循环
            }
        }
    }
}


/***************************************************
            画图函数
//x1,x2,y1,y2:显示区域对角坐标
//mode:模式1正常显示，0反白显示
//BMP[]:显示图形数组
//每页从上至下(逆向),列行式扫描
***************************************************/
void OLED_ShowBMP(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode,const unsigned char BMP[])
{
    u32 i;
    u8 j;
    u32 n;      //数组BMP元素个数
    u8 tempBMP=0;
    u8 xtemp=x1;
    
    if(x1>x2 || y1>y2)  return;//规划区域有问题，直接返回主函数
    n=(x2-x1) * ( (y2-y1)/8 + ((y2-y1)%8?1:0) );        //数组BMP元素个数
    for(i=0;i<n;i++)
    {
        tempBMP=BMP[i];     //取出相应显示值
        for(j=0;j<8;j++)
        {
            if(tempBMP&0x01)    OLED_DrawPiont(x1,y1+j,mode);   //判断最低位
            else                OLED_DrawPiont(x1,y1+j,!mode);
            tempBMP>>=1;
        }
        x1++;           //列地址加1
        if(x1==x2)      //列地址到达边界
        {
            x1=xtemp;   //回到起始列地址
            y1=y1+8;    //跳转到下一页
        }
    }
}



