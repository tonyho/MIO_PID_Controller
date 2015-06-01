#ifndef __ILI932X_H
#define __ILI932X_H

//定义驱动IC是ILI9320还是ILI9325
#define  ILI9325 

//屏幕旋转定义 数字按照 ID[1:0]AM 按照PDF中的配置定义
#define ID_AM  110

//屏幕开始时显示方式，注意：当IDelay时显示第一幅画面是逐像素刷新的
//此时必须手动在刷新结束后加上  LCD_WR_REG(0x0007,0x0173);才能显示
//当Immediately时没有被注释掉是不需要此过程
//#define Delay //Display Delay//Immediately

//硬件相关的子函数
#define Set_Cs  GPIO_SetBits(GPIOC,GPIO_Pin_8);
#define Clr_Cs  GPIO_ResetBits(GPIOC,GPIO_Pin_8);

#define Set_Rs  GPIO_SetBits(GPIOC,GPIO_Pin_9);
#define Clr_Rs  GPIO_ResetBits(GPIOC,GPIO_Pin_9);

#define Set_nWr GPIO_SetBits(GPIOC,GPIO_Pin_10);
#define Clr_nWr GPIO_ResetBits(GPIOC,GPIO_Pin_10);

#define Set_nRd GPIO_SetBits(GPIOC,GPIO_Pin_11);
#define Clr_nRd GPIO_ResetBits(GPIOC,GPIO_Pin_11);

#define Set_Rst GPIO_SetBits(GPIOC,GPIO_Pin_12);
#define Clr_Rst GPIO_ResetBits(GPIOC,GPIO_Pin_12);

#define Lcd_Light_ON   GPIO_SetBits(GPIOC,GPIO_Pin_13);
#define Lcd_Light_OFF  GPIO_ResetBits(GPIOC,GPIO_Pin_13);

typedef union
{
  u16 U16;
  u8 U8[2];
}ColorTypeDef;


void Lcd_Configuration(void);
void DataToWrite(u16 data);

//Lcd初始化及其低级控制函数
void Lcd_Initialize(void);
void LCD_WR_REG(u16 Index,u16 CongfigTemp);
void Lcd_WR_Start(void);
//Lcd高级控制函数
void Lcd_SetCursor(u8 x,u16 y);
void Lcd_Clear(u16 Color);
void Lcd_ClearCharBox(u8 x,u16 y,u16 Color);
void Lcd_SetBox(u8 xStart,u16 yStart,u8 xLong,u16 yLong,u16 x_offset,u16 y_offset);
void Lcd_ColorBox(u8 x,u16 y,u8 xLong,u16 yLong,u16 Color);
void Lcd_WriteASCII(u8 x,u8 y,u16 x_offset,u16 y_offset,u16 CharColor,u16 CharBackColor,u8 ASCIICode);
void Lcd_WriteASCIIClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u8 ASCIICode);
void Lcd_Write32X16ASCII(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,u8 ASCIICode);
void Lcd_Write32X16ASCIIClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColoru8,u8 ASCIICode);
void Lcd_Write32X16ASCIIWrite(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,u8 ASCIICode);
void Lcd_WriteString(u8 x,u8 y,u16 x_offset,u16 y_offset,u16 CharColor,u16 CharBackColor,char *s);
void Lcd_Write32X16String(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,char *s);
void Lcd_WriteStringClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,char *s);
void Lcd_Write32X16StringClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,char *s);

void Lcd_WriteChinese(u8 x,u8 y,u16 x_offset,u16 y_offset,u16 CharColor,u16 CharBackColor,u16 ChineseCode);
void Lcd_WriteChineseClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 ChineseCode);
void Lcd_Write32X32Chinese(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,u16 ChineseCode);
void Lcd_Write32X32ChineseClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 ChineseCode);
void Lcd_WriteChineseString(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,char *s);
void Lcd_WriteChineseStringClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,char *s);
void Lcd_Write32X32ChineseString(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,char *s);
void Lcd_Write32X32ChineseStringClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,char *s);
void LcdWritePictureFromSPI(u8 xStart,u16 yStart,u8 xLong,u16 yLong,u32 BaseAddr);

void GetASCIICode(u8* pBuffer,u8 ASCII,u32 BaseAddr);
void GetChineseCode(u8* pBuffer,u16 ChineseCode,u32 BaseAddr);
void Get320240PictureCode(u8* pBuffer,u32 BufferCounter,u32 BaseAddr);

void Delay_nms(int n);
//u16 Read9325(void);

/*定义常见颜色*/
#define red 0x001f
#define blue 0xf800
#define green 0x07e0
#define black 0x0000
#define white 0xffff
#define yellow 0x07ff
#define orange 0x05bf
#define Chocolate4 0x4451
#define Grey 0xefbd//灰色


//定义FLASH中的数据首地址
#define logo 0x51000
#define key24048 0x4a000
#define key8048 0x48000
#define key2480 0x77000
#define key2448 0x78000
#define BatteryHight 0x79000
#define BatteryMiddle 0x7a000
#define BatteryLow 0x7b000
#define BatteryTooLow 0x7c000
#define key24032 0x80000 
#define PowOffkey24048 0x84000

#define ASCII_Offset 0x47000
#define Chinese_Offset 0x1000

#endif

