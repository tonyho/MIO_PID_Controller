/******************************************************************************
* 文件名称：ili932x.c
* 摘 要：支持ILI9320和ILI9325驱动IC控制的QVGA显示屏，使用16位并行传输
  到头文件中配置 屏幕使用方向和驱动IC类型
  注意：16位数据线色彩分布>>  BGR(565)

* 当前版本：V1.3
* 修改说明：版本修订说明：
  1.修改翻转模式下的ASCII字符写Bug
  2.增加可以在翻转模式下的自动行写
  3.优化刷图片 使用流水线方法提效率
*重要说明！
在.h文件中，#define Immediately时是立即显示当前画面
而如果#define Delay，则只有在执行了LCD_WR_REG(0x0007,0x0173);
之后才会显示，执行一次LCD_WR_REG(0x0007,0x0173)后，所有写入数
据都立即显示。
#define Delay一般用在开机画面的显示，防止显示出全屏图像的刷新
过程
******************************************************************************/
#include "stm32f10x_lib.h"
#include "ili932x.h"
 
void DataToWrite(u16 data) 
{
	u16 temp;
	temp = GPIO_ReadOutputData(GPIOB);
	GPIO_Write(GPIOB, (data<<8)|(temp&0x00ff));
	temp = GPIO_ReadOutputData(GPIOC);
	GPIO_Write(GPIOC, (data>>8)|(temp&0xff00));
}

/****************************************************************************
* 名    称：u16 CheckController(void)
* 功    能：返回控制器代码
* 入口参数：无
* 出口参数：控制器型号
* 说    明：调用后返回兼容型号的控制器型号
* 调用方法：code=CheckController();
****************************************************************************/
u16 CheckController(void)
{
  	u16 tmp=0,tmp1=0,tmp2=0; 
	GPIO_InitTypeDef GPIO_InitStructure;

  	DataToWrite(0xffff);//数据线全高
	Set_Rst;
	Set_nWr;
	Set_Cs;
	Set_Rs;
	Set_nRd;
	Set_Rst;
	Delay_nms(1);
	Clr_Rst;
	Delay_nms(1);
	Set_Rst;
	Delay_nms(1);
	LCD_WR_REG(0x0000,0x0001);  //start oscillation
	Delay_nms(1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*16位数据高8位*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  	GPIO_ResetBits(GPIOC,GPIO_Pin_8);
  
  	GPIO_SetBits(GPIOC,GPIO_Pin_9);
  
  	GPIO_ResetBits(GPIOC,GPIO_Pin_11);

  	tmp1 = GPIO_ReadInputData(GPIOB);
	tmp2 = GPIO_ReadInputData(GPIOC);

	tmp = (tmp1>>8) | (tmp2<<8);
  
  	GPIO_SetBits(GPIOC,GPIO_Pin_11);
  
  	GPIO_SetBits(GPIOC,GPIO_Pin_8);

	/*16位数据低8位*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*16位数据高8位*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  	return tmp;
}

/**********************************************
函数名：Lcd初始化函数
功能：初始化Lcd
入口参数：无
返回值：无
***********************************************/
void Lcd_Initialize(void)
{
  	u16 i;
	Lcd_Light_ON;
	DataToWrite(0xffff);//数据线全高
	Set_Rst;
	Set_nWr;
	Set_Cs;
	Set_Rs;
	Set_nRd;
	Set_Rst;
	Delay_nms(1);
	Clr_Rst;
	Delay_nms(1);
	Set_Rst;
	Delay_nms(1); 
	
	i = CheckController();
	if(i==0x9325)
	{
  		LCD_WR_REG(0x00e7,0x0010);      
        LCD_WR_REG(0x0000,0x0001);  			//start internal osc
        LCD_WR_REG(0x0001,0x0100);     
        LCD_WR_REG(0x0002,0x0700); 				//power on sequence                     
        LCD_WR_REG(0x0003,(1<<12)|(1<<5)|(1<<4) ); 	//65K 
        LCD_WR_REG(0x0004,0x0000);                                   
        LCD_WR_REG(0x0008,0x0207);	           
        LCD_WR_REG(0x0009,0x0000);         
        LCD_WR_REG(0x000a,0x0000); 				//display setting         
        LCD_WR_REG(0x000c,0x0001);				//display setting          
        LCD_WR_REG(0x000d,0x0000); 				//0f3c          
        LCD_WR_REG(0x000f,0x0000);
        LCD_WR_REG(0x0010,0x0000);   
        LCD_WR_REG(0x0011,0x0007);
        LCD_WR_REG(0x0012,0x0000);                                                                 
        LCD_WR_REG(0x0013,0x0000);                 
        for(i=50000;i>0;i--);
        LCD_WR_REG(0x0010,0x1590);   
        LCD_WR_REG(0x0011,0x0227);
        for(i=50000;i>0;i--);
        LCD_WR_REG(0x0012,0x009c);                 
        for(i=50000;i>0;i--);
        LCD_WR_REG(0x0013,0x1900);   
        LCD_WR_REG(0x0029,0x0023);
        LCD_WR_REG(0x002b,0x000e);
        for(i=50000;i>0;i--);
        LCD_WR_REG(0x0020,0x0000);                                                            
        LCD_WR_REG(0x0021,0x0000);                 
      
        for(i=50000;i>0;i--);
        LCD_WR_REG(0x0030,0x0007); 
        LCD_WR_REG(0x0031,0x0707);   
        LCD_WR_REG(0x0032,0x0006);
        LCD_WR_REG(0x0035,0x0704);
        LCD_WR_REG(0x0036,0x1f04); 
        LCD_WR_REG(0x0037,0x0004);
        LCD_WR_REG(0x0038,0x0000);        
        LCD_WR_REG(0x0039,0x0706);     
        LCD_WR_REG(0x003c,0x0701);
        LCD_WR_REG(0x003d,0x000f);
        for(i=50000;i>0;i--);
        LCD_WR_REG(0x0050,0x0000);        
        LCD_WR_REG(0x0051,0x00ef);   
        LCD_WR_REG(0x0052,0x0000);     
        LCD_WR_REG(0x0053,0x013f);
        LCD_WR_REG(0x0060,0xa700);        
        LCD_WR_REG(0x0061,0x0001); 
        LCD_WR_REG(0x006a,0x0000);
        LCD_WR_REG(0x0080,0x0000);
        LCD_WR_REG(0x0081,0x0000);
        LCD_WR_REG(0x0082,0x0000);
        LCD_WR_REG(0x0083,0x0000);
        LCD_WR_REG(0x0084,0x0000);
        LCD_WR_REG(0x0085,0x0000);
      
        LCD_WR_REG(0x0090,0x0010);     
        LCD_WR_REG(0x0092,0x0000);  
        LCD_WR_REG(0x0093,0x0003);
        LCD_WR_REG(0x0095,0x0110);
        LCD_WR_REG(0x0097,0x0000);        
        LCD_WR_REG(0x0098,0x0000);  
         //display on sequence     
        LCD_WR_REG(0x0007,0x0133);
    
        LCD_WR_REG(0x0020,0x0000);                                                            
        LCD_WR_REG(0x0021,0x0000);
	}
	else if(i==0x9320)
	{
		LCD_WR_REG(0x00,0x0000);
		LCD_WR_REG(0x01,0x0100);	//Driver Output Contral.
		LCD_WR_REG(0x02,0x0700);	//LCD Driver Waveform Contral.
		LCD_WR_REG(0x03,0x1030);	//Entry Mode Set.
	
		LCD_WR_REG(0x04,0x0000);	//Scalling Contral.
		LCD_WR_REG(0x08,0x0202);	//Display Contral 2.(0x0207)
		LCD_WR_REG(0x09,0x0000);	//Display Contral 3.(0x0000)
		LCD_WR_REG(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
		LCD_WR_REG(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
		LCD_WR_REG(0x0d,0x0000);	//Frame Maker Position.
		LCD_WR_REG(0x0f,0x0000);	//Extern Display Interface Contral 2.
	
		for(i=50000;i>0;i--);
		LCD_WR_REG(0x07,0x0101);	//Display Contral.
		for(i=50000;i>0;i--);
	
		LCD_WR_REG(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	//Power Control 1.(0x16b0)
		LCD_WR_REG(0x11,0x0007);								//Power Control 2.(0x0001)
		LCD_WR_REG(0x12,(1<<8)|(1<<4)|(0<<0));					//Power Control 3.(0x0138)
		LCD_WR_REG(0x13,0x0b00);								//Power Control 4.
		LCD_WR_REG(0x29,0x0000);								//Power Control 7.
	
		LCD_WR_REG(0x2b,(1<<14)|(1<<4));
		
		LCD_WR_REG(0x50,0);		//Set X Start.
		LCD_WR_REG(0x51,239);	//Set X End.
		LCD_WR_REG(0x52,0);		//Set Y Start.
		LCD_WR_REG(0x53,319);	//Set Y End.
	
		LCD_WR_REG(0x60,0x2700);	//Driver Output Control.
		LCD_WR_REG(0x61,0x0001);	//Driver Output Control.
		LCD_WR_REG(0x6a,0x0000);	//Vertical Srcoll Control.
	
		LCD_WR_REG(0x80,0x0000);	//Display Position? Partial Display 1.
		LCD_WR_REG(0x81,0x0000);	//RAM Address Start? Partial Display 1.
		LCD_WR_REG(0x82,0x0000);	//RAM Address End-Partial Display 1.
		LCD_WR_REG(0x83,0x0000);	//Displsy Position? Partial Display 2.
		LCD_WR_REG(0x84,0x0000);	//RAM Address Start? Partial Display 2.
		LCD_WR_REG(0x85,0x0000);	//RAM Address End? Partial Display 2.
	
		LCD_WR_REG(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
		LCD_WR_REG(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
		LCD_WR_REG(0x93,0x0001);	//Panel Interface Contral 3.
		LCD_WR_REG(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
		LCD_WR_REG(0x97,(0<<8));	//
		LCD_WR_REG(0x98,0x0000);	//Frame Cycle Contral.

	
		LCD_WR_REG(0x07,0x0173);	//(0x0173)
	}
}


/******************************************
函数名：Lcd写命令函数
功能：向Lcd指定位置写入应有命令或数据
入口参数：Index 要寻址的寄存器地址
          ConfigTemp 写入的数据或命令值
返回值：无
******************************************/
void LCD_WR_REG(u16 Index,u16 CongfigTemp)
{
	Clr_Cs;
	Clr_Rs;
	Set_nRd;
	DataToWrite(Index);
	Clr_nWr;
	Set_nWr;
	Set_Rs;       
	DataToWrite(CongfigTemp);       
	Clr_nWr;
	Set_nWr;
	Set_Cs;
}


/************************************************
函数名：Lcd写开始函数
功能：控制Lcd控制引脚 执行写操作
入口参数：无
返回值：无
************************************************/
void Lcd_WR_Start(void)
{
	Clr_Cs;
	Clr_Rs;
	Set_nRd;
	DataToWrite(0x0022);
	Clr_nWr;
	Set_nWr;
	Set_Rs;
}


/*************************************************
函数名：Lcd光标起点定位函数
功能：指定320240液晶上的一点作为写数据的起始点
入口参数：x 坐标 0~239
          y 坐标 0~319
返回值：无
*************************************************/
void Lcd_SetCursor(u8 x,u16 y)
{ 
	LCD_WR_REG(0x20,x);
	LCD_WR_REG(0x21,y);    
}


/**********************************************
函数名：Lcd全屏擦除函数
功能：将Lcd整屏擦为指定颜色
入口参数：color 指定Lcd全屏颜色 RGB(5-6-5)
返回值：无
***********************************************/
void Lcd_Clear(u16 Color)
{
	u32 temp;
  
	Lcd_SetCursor(0x00, 0x0000);
	LCD_WR_REG(0x0050,0x00);//水平 GRAM起始位置
	LCD_WR_REG(0x0051,239);//水平GRAM终止位置
	LCD_WR_REG(0x0052,0x00);//垂直GRAM起始位置
	LCD_WR_REG(0x0053,319);//垂直GRAM终止位置   
	Lcd_WR_Start();
	Set_Rs;
  
	for (temp = 0; temp < 76800; temp++)
	{
		DataToWrite(Color);
		Clr_nWr;
		Set_nWr;
	}
  
	Set_Cs;
}
/**********************************************
函数名：Lcd块选函数
功能：选定Lcd上指定的矩形区域

注意：xStart和 yStart随着屏幕的旋转而改变，位置是矩形框的四个角

入口参数：xStart x方向的起始点
          ySrart y方向的终止点
          xLong 要选定矩形的x方向长度
          yLong  要选定矩形的y方向长度
返回值：无
***********************************************/
void Lcd_SetBox(u8 xStart,u16 yStart,u8 xLong,u16 yLong,u16 x_offset,u16 y_offset)
{
  
#if ID_AM==000    
	Lcd_SetCursor(xStart+xLong-1+x_offset,yStart+yLong-1+y_offset);

#elif ID_AM==001
	Lcd_SetCursor(xStart+xLong-1+x_offset,yStart+yLong-1+y_offset);
     
#elif ID_AM==010
	Lcd_SetCursor(xStart+x_offset,yStart+yLong-1+y_offset);
     
#elif ID_AM==011 
	Lcd_SetCursor(xStart+x_offset,yStart+yLong-1+y_offset);
     
#elif ID_AM==100
	Lcd_SetCursor(xStart+xLong-1+x_offset,yStart+y_offset);     
     
#elif ID_AM==101
	Lcd_SetCursor(xStart+xLong-1+x_offset,yStart+y_offset);     
     
#elif ID_AM==110
	Lcd_SetCursor(xStart+x_offset,yStart+y_offset); 
     
#elif ID_AM==111
	Lcd_SetCursor(xStart+x_offset,yStart+y_offset);  
     
#endif
     
	LCD_WR_REG(0x0050,xStart+x_offset);//水平 GRAM起始位置
	LCD_WR_REG(0x0051,xStart+xLong-1+x_offset);//水平GRAM终止位置
	LCD_WR_REG(0x0052,yStart+y_offset);//垂直GRAM起始位置
	LCD_WR_REG(0x0053,yStart+yLong-1+y_offset);//垂直GRAM终止位置 
}


void Lcd_ColorBox(u8 xStart,u16 yStart,u8 xLong,u16 yLong,u16 Color)
{
	u32 temp;
  
	Lcd_SetBox(xStart,yStart,xLong,yLong,0,0);
	Lcd_WR_Start();
	Set_Rs;
  
	for (temp=0; temp<xLong*yLong; temp++)
	{
		DataToWrite(Color);
		Clr_nWr;
		Set_nWr;
	}

	Set_Cs;
}


void Lcd_ClearCharBox(u8 x,u16 y,u16 Color)
{
	u32 temp;
  
	Lcd_SetBox(x*8,y*16,8,16,0,0); 
	Lcd_WR_Start();
	Set_Rs;
  
	for (temp=0; temp < 128; temp++)
	{
		DataToWrite(Color); 
		Clr_nWr;
		//Delay_nus(22);
		Set_nWr; 
	}
	
	Set_Cs;
}

void Delay_nms(int n)
{
  
  u32 f=n,k;
  for (; f!=0; f--)
  {
    for(k=0xFFF; k!=0; k--);
  }
  
}

void test_color(){
  u8  R_data,G_data,B_data,i,j;

	Lcd_SetCursor(0x00, 0x0000);
	LCD_WR_REG(0x0050,0x00);//水平 GRAM起始位置
	LCD_WR_REG(0x0051,239);//水平GRAM终止位置
	LCD_WR_REG(0x0052,0);//垂直GRAM起始位置
	LCD_WR_REG(0x0053,319);//垂直GRAM终止位置   
	Lcd_WR_Start();
	Set_Rs;
    R_data=0;G_data=0;B_data=0;     
    for(j=0;j<50;j++)//红色渐强条
    {
        for(i=0;i<240;i++)
            {R_data=i/8;DataToWrite(R_data<<11|G_data<<5|B_data);
			Clr_nWr;Set_nWr;}
    }
    R_data=0x1f;G_data=0x3f;B_data=0x1f;
    for(j=0;j<50;j++)
    {
        for(i=0;i<240;i++)
            {
            G_data=0x3f-(i/4);
            B_data=0x1f-(i/8);
            DataToWrite(R_data<<11|G_data<<5|B_data);
			Clr_nWr;
			Set_nWr;
			}
    }
//----------------------------------
    R_data=0;G_data=0;B_data=0;
    for(j=0;j<50;j++)//绿色渐强条
    {
        for(i=0;i<240;i++)
            {G_data=i/4;
			DataToWrite(R_data<<11|G_data<<5|B_data);
			Clr_nWr;
			Set_nWr;}
    }

    R_data=0x1f;G_data=0x3f;B_data=0x1f;
    for(j=0;j<50;j++)
    {
        for(i=0;i<240;i++)
            {
            R_data=0x1f-(i/8);
            B_data=0x1f-(i/8);
            DataToWrite(R_data<<11|G_data<<5|B_data);
			Clr_nWr;
			Set_nWr;
		}
    }
//----------------------------------
 
    R_data=0;G_data=0;B_data=0;
    for(j=0;j<60;j++)//蓝色渐强条
    {
        for(i=0;i<240;i++)
            {B_data=i/8;DataToWrite(R_data<<11|G_data<<5|B_data);
			Clr_nWr;
			Set_nWr;}
    } 

    B_data=0; 
    R_data=0x1f;G_data=0x3f;B_data=0x1f;

    for(j=0;j<60;j++)
    {
        for(i=0;i<240;i++)
            {
            G_data=0x3f-(i/4);
            R_data=0x1f-(i/8);
            DataToWrite(R_data<<11|G_data<<5|B_data);
			Clr_nWr;
			Set_nWr;
		}
    }	  
	Set_Cs;
}
//====================================================================================
