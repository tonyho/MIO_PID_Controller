#include "stm32f10x_lib.h"
#include "ili9320.h"

#define CH_X  0xd0//0x90
#define CH_Y  0x90//0xd0

/*
for stmfans-ev board
lcd:
PD0 -> nCS  PD1 -> RS  PD2 -> nWR  PD3 -> nRD  PD4 -> nRESET
touch:
PD5 -> TS_CLK  PD6 -> TS_nCS  PD7 -> TS_DIN  PD8 -> TS_BUSY  PD9 -> TS_nPENIRQ  PD10 -> TS_DOUT
*/


void SPI_CS(u8 a)
{
  // PD6 -> TS_nCS
  if (a)
    GPIO_SetBits(GPIOD,GPIO_Pin_6);
  else
    GPIO_ResetBits(GPIOD,GPIO_Pin_6);
}

void SPI_DIN(u8 a)
{
  // PD7 -> TS_DIN
  if (a)
    GPIO_SetBits(GPIOD,GPIO_Pin_7);
  else
    GPIO_ResetBits(GPIOD,GPIO_Pin_7);
}

void SPI_CLK(u8 a)
{
  // PD5 -> TS_CLK
  if (a)
    GPIO_SetBits(GPIOD,GPIO_Pin_5);
  else
    GPIO_ResetBits(GPIOD,GPIO_Pin_5);
}

u8 SPI_DOUT(void)
{
  // PD10 -> TS_DOUT
  return GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_10);
}

void SPI_delay(u16 i)
{
  u16 k;
  for (k=0;k<i;k++);
}

void Touch_Start(void)
{
	SPI_CLK(0);
	SPI_CS(1);
	SPI_DIN(1);
	SPI_CLK(1);
	SPI_CS(0);
}

void Touch_Write(u8 d)
{
	u8 buf, i ;
	
  SPI_CLK(0);
	for( i = 0; i < 8; i++ )
	{
    buf = (d >> (7-i)) & 0x1 ;
    SPI_DIN(buf);
    SPI_CLK(0);
    SPI_CLK(1);
    SPI_CLK(0);
	}
}

u16  Touch_Read(void)
{
	u16 buf ;
	u8 i ;

	buf=0;
	for( i = 0; i < 12; i++ )
	{
		buf = buf << 1 ;
		SPI_CLK(1);
		SPI_CLK(0);			
		if ( SPI_DOUT() )	
		{
			buf = buf + 1 ;
		}
	}
	return( buf ) ;
}

u8  Touch_Busy(void)
{
  // PD8 -> TS_BUSY
  return GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_8);
}

u8  Touch_PenIRQ(void)
{
  // PD9 -> TS_nPENIRQ
  return GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_9);
}

void Touch_Initializtion()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*****************************
  **    硬件连接说明          **
  ** STM32         TSC2046    **
  ** PD3    <----> nPENIRQ    ** i
  ** PD4    <----> BUSY       ** i
  ** PC6    <----> DCLK       ** o
  ** PC5    <----> DIN        ** o
  ** PC4    <----> DOUT       ** i
  ** PC7    <----> nCS        ** o
  ******************************/
// PD5 -> TS_CLK  PD6 -> TS_nCS  PD7 -> TS_DIN  PD8 -> TS_BUSY  PD9 -> TS_nPENIRQ  PD10 -> TS_DOUT

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}

u16 _AD2X(int adx)
{
  u16 sx=0;
  int r = adx - 280;
  r *= 239;
  sx=r / (3740 - 280);
  if (sx<=0 || sx>240)
    return 0;
  return sx;
}

u16 _AD2Y(int ady)
{
  u16 sy=0;
  int r = ady - 230;
  r *= 319;
  sy=r/(3720 - 230);
  if (sy<=0 || sy>320)
    return 0;
  return sy;
}

u16  Touch_MeasurementX(void)
{
  u8 i;
  u16 p=0;
  for (i=0;i<8;i++)
  {
    p+=Touch_GetPhyX();
    SPI_delay(1000);
  }
  p>>=3;
  
  return ( p );
}

u16  Touch_MeasurementY(void)
{
  u8 i;
  u16 p=0;
  for (i=0;i<8;i++)
  {
    p+=Touch_GetPhyY();
    SPI_delay(1000);
  }
  p>>=3;
  
  return ( p );
}

u16  Touch_GetPhyX(void)
{
  if (Touch_PenIRQ()) return 0;
  Touch_Start();
  Touch_Write(0x00);
  Touch_Write(CH_X);
  while(!Touch_Busy());
  return (Touch_Read());
}

u16  Touch_GetPhyY(void)
{
  if (Touch_PenIRQ()) return 0;
  Touch_Start();
  Touch_Write(0x00);
  Touch_Write(CH_Y);
  while(!Touch_Busy());
  return (Touch_Read());
}
/*
u16 Dx(u16 xx)
{
  return (_AD2X(xx));
}
u16 Dy(u16 yy)
{
  return (_AD2Y(yy));
}
*/
