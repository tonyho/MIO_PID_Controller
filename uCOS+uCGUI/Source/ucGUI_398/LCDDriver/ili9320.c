#include "stm32f10x_lib.h"
#include "ili9320.h"
#include "ili9320_font.h"

static u16 DeviceCode;		/* 显示器类型 */
static u8 g_Is4001 = 0;	/* 区别是5420屏还是4001屏 */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  vu16 LCD_REG;
  vu16 LCD_RAM;
} LCD_TypeDef;

/* LCD is connected to the FSMC_Bank1_NOR/SRAM4 and NE4 is used as ship select signal */
#define LCD_BASE    ((u32)(0x60000000 | 0x0C000000))
#define LCD         ((LCD_TypeDef *) LCD_BASE)
////////////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* Function Name  : LCD_CtrlLinesConfig
* Description    : Configures LCD Control lines (FSMC Pins) in alternate function
                   Push-Pull mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_CtrlLinesConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable FSMC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |
                         RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG |
                         RCC_APB2Periph_AFIO, ENABLE);

  /* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
     PD.10(D15), PD.14(D0), PD.15(D1) as alternate
     function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 |
                                GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) as alternate function push pull */
  /* 2009-11-14 armfly : STM32F103ZE-EK开发板使用了PE3,PE4做地址线，必须使能 */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                GPIO_Pin_15 | GPIO_Pin_5 | GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_WriteBit(GPIOE, GPIO_Pin_6, Bit_SET);
  /* Set PF.00(A0 (RS)) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* Set PG.12(NE4 (LCD/CS)) as alternate function push pull - CE3(LCD /CS) */
  /* PG10 (NE30 for SRAM */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_10;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : LCD_FSMCConfig
* Description    : Configures the Parallel interface (FSMC) for LCD(Parallel mode)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_FSMCConfig(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;

/*-- FSMC Configuration ------------------------------------------------------*/
/*----------------------- SRAM Bank 4 ----------------------------------------*/
  /* FSMC_Bank1_NORSRAM4 configuration */
  p.FSMC_AddressSetupTime = 0;
  p.FSMC_AddressHoldTime = 0;

  /* LCD 读需要设置为4，只写的话设置1即可 */
  p.FSMC_DataSetupTime = 4;

  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_A;

  /* Color LCD configuration ------------------------------------
     LCD configured as follow:
        - Data/Address MUX = Disable
        - Memory Type = SRAM
        - Data Width = 16bit
        - Write Operation = Enable
        - Extended Mode = Enable
        - Asynchronous Wait = Disable */
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsyncWait = FSMC_AsyncWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

  /* BANK 4 (of NOR/SRAM Bank 1~4) is enabled */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}

void LCD_X_Init(void)
{
 /* Configure the LCD Control pins --------------------------------------------*/
  LCD_CtrlLinesConfig();

/* Configure the FSMC Parallel interface -------------------------------------*/
  LCD_FSMCConfig();
}


/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{
  /* Write 16-bit Index, then Write Reg */
  LCD->LCD_REG = LCD_Reg;
  /* Write 16-bit Reg */
  LCD->LCD_RAM = LCD_RegValue;
}

/*******************************************************************************
* Function Name  : LCD_ReadReg
* Description    : Reads the selected LCD Register.
* Input          : None
* Output         : None
* Return         : LCD Register Value.
*******************************************************************************/
u16 LCD_ReadReg(u16 LCD_Reg)
{
  /* Write 16-bit Index (then Read Reg) */
  LCD->LCD_REG = LCD_Reg;
  /* Read 16-bit Reg */
  return (LCD->LCD_RAM);
}

/*******************************************************************************
* Function Name  : LCD_WriteRAM_Prepare
* Description    : Prepare to write to the LCD RAM.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
	if (DeviceCode == 0x5420)
	{
		LCD->LCD_REG = 0x202;
	}
	else
	{
		LCD->LCD_REG = R34;
	}
}

/*******************************************************************************
* Function Name  : LCD_WriteRAM
* Description    : Writes to the LCD RAM.
* Input          : - RGB_Code: the pixel color in RGB mode (5-6-5).
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRAM(u16 RGB_Code)
{
  /* Write 16-bit GRAM Reg */
  LCD->LCD_RAM = RGB_Code;
}

/*******************************************************************************
* Function Name  : LCD_ReadRAM
* Description    : Reads the LCD RAM.
* Input          : None
* Output         : None
* Return         : LCD RAM Value.
*******************************************************************************/
u16 LCD_ReadRAM(void)
{
	u16 dummy;
	/* Write 16-bit Index (then Read Reg) */
	if (DeviceCode == 0x5420)
	{
		LCD->LCD_REG = 0x202; /* Select GRAM Reg */
	}
	else
	{
		LCD->LCD_REG = R34; /* Select GRAM Reg */;
	}
	/* Read 16-bit Reg */
	dummy = LCD->LCD_RAM;
	return LCD->LCD_RAM;
}

/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position.
* Output         : None
* Return         : None
*******************************************************************************/


void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
	if (DeviceCode == 0x5420)
	{
		LCD_WriteReg(0x0200, Xpos); //StartX);
		LCD_WriteReg(0x0201, Ypos);//StartY;
	}
	else
	{
		LCD_WriteReg(0x06,Ypos>>8);
		LCD_WriteReg(0x07,Ypos);

		LCD_WriteReg(0x02,Xpos>>8);
		LCD_WriteReg(0x03,Xpos);
	}
}


void Delay(u32 nCount)
{
 u32 TimingDelay;
 while(nCount--)
   {
    for(TimingDelay=0;TimingDelay<10000;TimingDelay++);
   }
}

/****************************************************************************
* 名    称：void ili9320_Initializtion()
* 功    能：初始化 ILI9320 控制器
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：ili9320_Initializtion();
****************************************************************************/
void ili9320_Initializtion()
{
  /*****************************
  **    硬件连接说明          **
  ** STM32         ili9320    **
  ** PE0~15 <----> DB0~15     **
  ** PD15   <----> nRD        **
  ** PD14   <----> RS         **
  ** PD13   <----> nWR        **
  ** PD12   <----> nCS        **
  ** PD11   <----> nReset     **
  ** PC0    <----> BK_LED     **
  ******************************/
  u16 i;
  LCD_X_Init();
  Delay(5); /* delay 50 ms */
//  LCD_WriteReg(0x0000,0x0001);
  Delay(5); /* delay 50 ms */			//start internal osc
  DeviceCode = LCD_ReadReg(0x0000);
  DeviceCode = 0X5420;
	if (DeviceCode == 0x5420)
	{
		LCD_WriteReg(0x0001, 0x0100);
		LCD_WriteReg(0x0002, 0x0100);

		LCD_WriteReg(0x0003, 0x1008); /* 1030 ,  1008, 1018 */

		LCD_WriteReg(0x0008, 0x0808);
		LCD_WriteReg(0x0009, 0x0001);
		LCD_WriteReg(0x000B, 0x0010);
		LCD_WriteReg(0x000C, 0x0000);
		LCD_WriteReg(0x000F, 0x0000);
		LCD_WriteReg(0x0007, 0x0001);
		LCD_WriteReg(0x0010, 0x0013);
		LCD_WriteReg(0x0011, 0x0501);
		LCD_WriteReg(0x0012, 0x0300);
		LCD_WriteReg(0x0020, 0x021E);
		LCD_WriteReg(0x0021, 0x0202);
		LCD_WriteReg(0x0090, 0x8000);
		LCD_WriteReg(0x0100, 0x17B0);
		LCD_WriteReg(0x0101, 0x0147);
		LCD_WriteReg(0x0102, 0x0135);
		LCD_WriteReg(0x0103, 0x0700);
		LCD_WriteReg(0x0107, 0x0000);
		LCD_WriteReg(0x0110, 0x0001);
		LCD_WriteReg(0x0210, 0x0000);
		LCD_WriteReg(0x0211, 0x00EF);
		LCD_WriteReg(0x0212, 0x0000);
		LCD_WriteReg(0x0213, 0x018F);
		LCD_WriteReg(0x0280, 0x0000);
		LCD_WriteReg(0x0281, 0x0004);
		LCD_WriteReg(0x0282, 0x0000);
		LCD_WriteReg(0x0300, 0x0101);
		LCD_WriteReg(0x0301, 0x0B2C);
		LCD_WriteReg(0x0302, 0x1030);
		LCD_WriteReg(0x0303, 0x3010);
		LCD_WriteReg(0x0304, 0x2C0B);
		LCD_WriteReg(0x0305, 0x0101);
		LCD_WriteReg(0x0306, 0x0807);
		LCD_WriteReg(0x0307, 0x0708);
		LCD_WriteReg(0x0308, 0x0107);
		LCD_WriteReg(0x0309, 0x0105);
		LCD_WriteReg(0x030A, 0x0F04);
		LCD_WriteReg(0x030B, 0x0F00);
		LCD_WriteReg(0x030C, 0x000F);
		LCD_WriteReg(0x030D, 0x040F);
		LCD_WriteReg(0x030E, 0x0300);
		LCD_WriteReg(0x030F, 0x0701);
		LCD_WriteReg(0x0400, 0x3500);
		LCD_WriteReg(0x0401, 0x0001);
		LCD_WriteReg(0x0404, 0x0000);
		LCD_WriteReg(0x0500, 0x0000);
		LCD_WriteReg(0x0501, 0x0000);
		LCD_WriteReg(0x0502, 0x0000);
		LCD_WriteReg(0x0503, 0x0000);
		LCD_WriteReg(0x0504, 0x0000);
		LCD_WriteReg(0x0505, 0x0000);
		LCD_WriteReg(0x0600, 0x0000);
		LCD_WriteReg(0x0606, 0x0000);
		LCD_WriteReg(0x06F0, 0x0000);
		LCD_WriteReg(0x07F0, 0x5420);
		LCD_WriteReg(0x07DE, 0x0000);
		LCD_WriteReg(0x07F2, 0x00DF);
		LCD_WriteReg(0x07F3, 0x0810);
		LCD_WriteReg(0x07F4, 0x0077);
		LCD_WriteReg(0x07F5, 0x0021);
		LCD_WriteReg(0x07F0, 0x0000);
		LCD_WriteReg(0x0007, 0x0173);

		/* SET WINDOWS */
		LCD_WriteReg(0x0210, 0); // Horizontal GRAM Start Address
		LCD_WriteReg(0x0211, 239); // Horizontal GRAM End Address
		LCD_WriteReg(0x0212, 0); // Vertical GRAM Start Address
		LCD_WriteReg(0x0213, 399); // Vertical GRAM Start Address

		/* 下面这段代码用于识别是4001屏还是5420屏 */
		{
			u16 dummy;
			LCD_WriteReg(0x0200, 0); //StartX);
			LCD_WriteReg(0x0201, 0); //StartY);		
			LCD->LCD_REG = 0x0202;
			LCD->LCD_RAM = 0x1234;
	
			LCD_WriteReg(0x0200, 0); //StartX);
			LCD_WriteReg(0x0201, 0); //StartY);
			LCD->LCD_REG = 0x0202;
			dummy = LCD->LCD_RAM;
			if (LCD->LCD_RAM == 0x1234)
			{
				g_Is4001 = 1;
			}
			else
			{
				g_Is4001 = 0;
			}
		}		
	}
	else if(DeviceCode==0x9325||DeviceCode==0x9328)
	{
  		LCD_WriteReg(0x00e7,0x0010);
        LCD_WriteReg(0x0000,0x0001);  			//start internal osc
        LCD_WriteReg(0x0001,0x0100);
        LCD_WriteReg(0x0002,0x0700); 				//power on sequence
        LCD_WriteReg(0x0003,(1<<12)|(1<<5)|(1<<4) ); 	//65K
        LCD_WriteReg(0x0004,0x0000);
        LCD_WriteReg(0x0008,0x0207);
        LCD_WriteReg(0x0009,0x0000);
        LCD_WriteReg(0x000a,0x0000); 				//display setting
        LCD_WriteReg(0x000c,0x0001);				//display setting
        LCD_WriteReg(0x000d,0x0000); 				//0f3c
        LCD_WriteReg(0x000f,0x0000);
//Power On sequence //
        LCD_WriteReg(0x0010,0x0000);
        LCD_WriteReg(0x0011,0x0007);
        LCD_WriteReg(0x0012,0x0000);
        LCD_WriteReg(0x0013,0x0000);
        for(i=50000;i>0;i--);
		for(i=50000;i>0;i--);
        LCD_WriteReg(0x0010,0x1590);
        LCD_WriteReg(0x0011,0x0227);
        for(i=50000;i>0;i--);
		for(i=50000;i>0;i--);
        LCD_WriteReg(0x0012,0x009c);
        for(i=50000;i>0;i--);
		for(i=50000;i>0;i--);
        LCD_WriteReg(0x0013,0x1900);
        LCD_WriteReg(0x0029,0x0023);
        LCD_WriteReg(0x002b,0x000e);
        for(i=50000;i>0;i--);
		for(i=50000;i>0;i--);
        LCD_WriteReg(0x0020,0x0000);
        LCD_WriteReg(0x0021,0x0000);
///////////////////////////////////////////////////////
        for(i=50000;i>0;i--);
		for(i=50000;i>0;i--);
        LCD_WriteReg(0x0030,0x0007);
        LCD_WriteReg(0x0031,0x0707);
        LCD_WriteReg(0x0032,0x0006);
        LCD_WriteReg(0x0035,0x0704);
        LCD_WriteReg(0x0036,0x1f04);
        LCD_WriteReg(0x0037,0x0004);
        LCD_WriteReg(0x0038,0x0000);
        LCD_WriteReg(0x0039,0x0706);
        LCD_WriteReg(0x003c,0x0701);
        LCD_WriteReg(0x003d,0x000f);
        for(i=50000;i>0;i--);
		for(i=50000;i>0;i--);
        LCD_WriteReg(0x0050,0x0000);
        LCD_WriteReg(0x0051,0x00ef);
        LCD_WriteReg(0x0052,0x0000);
        LCD_WriteReg(0x0053,0x013f);
        LCD_WriteReg(0x0060,0xa700);
        LCD_WriteReg(0x0061,0x0001);
        LCD_WriteReg(0x006a,0x0000);
        LCD_WriteReg(0x0080,0x0000);
        LCD_WriteReg(0x0081,0x0000);
        LCD_WriteReg(0x0082,0x0000);
        LCD_WriteReg(0x0083,0x0000);
        LCD_WriteReg(0x0084,0x0000);
        LCD_WriteReg(0x0085,0x0000);

        LCD_WriteReg(0x0090,0x0010);
        LCD_WriteReg(0x0092,0x0000);
        LCD_WriteReg(0x0093,0x0003);
        LCD_WriteReg(0x0095,0x0110);
        LCD_WriteReg(0x0097,0x0000);
        LCD_WriteReg(0x0098,0x0000);
         //display on sequence
        LCD_WriteReg(0x0007,0x0133);

        LCD_WriteReg(0x0020,0x0000);
        LCD_WriteReg(0x0021,0x0000);
	}
	else if(DeviceCode==0x9320||DeviceCode==0x9300)
	{
		LCD_WriteReg(0x00,0x0000);
		LCD_WriteReg(0x01,0x0100);	//Driver Output Contral.
		LCD_WriteReg(0x02,0x0700);	//LCD Driver Waveform Contral.
//		LCD_WriteReg(0x03,0x1030);	//Entry Mode Set.
		LCD_WriteReg(0x03,0x1018);	//Entry Mode Set.

		LCD_WriteReg(0x04,0x0000);	//Scalling Contral.
		LCD_WriteReg(0x08,0x0202);	//Display Contral 2.(0x0207)
		LCD_WriteReg(0x09,0x0000);	//Display Contral 3.(0x0000)
		LCD_WriteReg(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
		LCD_WriteReg(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
		LCD_WriteReg(0x0d,0x0000);	//Frame Maker Position.
		LCD_WriteReg(0x0f,0x0000);	//Extern Display Interface Contral 2.

		for(i=50000;i>0;i--);
		for(i=50000;i>0;i--);
		LCD_WriteReg(0x07,0x0101);	//Display Contral.
		for(i=50000;i>0;i--);
		for(i=50000;i>0;i--);

		LCD_WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	//Power Control 1.(0x16b0)
		LCD_WriteReg(0x11,0x0007);								//Power Control 2.(0x0001)
		LCD_WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));					//Power Control 3.(0x0138)
		LCD_WriteReg(0x13,0x0b00);								//Power Control 4.
		LCD_WriteReg(0x29,0x0000);								//Power Control 7.

		LCD_WriteReg(0x2b,(1<<14)|(1<<4));

		LCD_WriteReg(0x50,0);		//Set X Start.
		LCD_WriteReg(0x51,239);	//Set X End.
		LCD_WriteReg(0x52,0);		//Set Y Start.
		LCD_WriteReg(0x53,319);	//Set Y End.

		LCD_WriteReg(0x60,0x2700);	//Driver Output Control.
		LCD_WriteReg(0x61,0x0001);	//Driver Output Control.
		LCD_WriteReg(0x6a,0x0000);	//Vertical Srcoll Control.

		LCD_WriteReg(0x80,0x0000);	//Display Position? Partial Display 1.
		LCD_WriteReg(0x81,0x0000);	//RAM Address Start? Partial Display 1.
		LCD_WriteReg(0x82,0x0000);	//RAM Address End-Partial Display 1.
		LCD_WriteReg(0x83,0x0000);	//Displsy Position? Partial Display 2.
		LCD_WriteReg(0x84,0x0000);	//RAM Address Start? Partial Display 2.
		LCD_WriteReg(0x85,0x0000);	//RAM Address End? Partial Display 2.

		LCD_WriteReg(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
		LCD_WriteReg(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
		LCD_WriteReg(0x93,0x0001);	//Panel Interface Contral 3.
		LCD_WriteReg(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
		LCD_WriteReg(0x97,(0<<8));	//
		LCD_WriteReg(0x98,0x0000);	//Frame Cycle Contral.


		LCD_WriteReg(0x07,0x0173);	//(0x0173)
	}
	else if(DeviceCode==0x9331)
	{
		LCD_WriteReg(0x00E7, 0x1014);
		LCD_WriteReg(0x0001, 0x0100); // set SS and SM bit   0x0100
		LCD_WriteReg(0x0002, 0x0200); // set 1 line inversion
		LCD_WriteReg(0x0003, 0x1030); // set GRAM write direction and BGR=1.     0x1030
		LCD_WriteReg(0x0008, 0x0202); // set the back porch and front porch
		LCD_WriteReg(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
		LCD_WriteReg(0x000A, 0x0000); // FMARK function
		LCD_WriteReg(0x000C, 0x0000); // RGB interface setting
		LCD_WriteReg(0x000D, 0x0000); // Frame marker Position
		LCD_WriteReg(0x000F, 0x0000); // RGB interface polarity
		//*************Power On sequence ****************//
		LCD_WriteReg(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
		LCD_WriteReg(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
		LCD_WriteReg(0x0012, 0x0000); // VREG1OUT voltage
		LCD_WriteReg(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
		ili9320_Delay(200); // Dis-charge capacitor power voltage
		LCD_WriteReg(0x0010, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
		LCD_WriteReg(0x0011, 0x0227); // DC1[2:0], DC0[2:0], VC[2:0]
		ili9320_Delay(50); // Delay 50ms
		LCD_WriteReg(0x0012, 0x000C); // Internal reference voltage= Vci;
		ili9320_Delay(50); // Delay 50ms
		LCD_WriteReg(0x0013, 0x0800); // Set VDV[4:0] for VCOM amplitude
		LCD_WriteReg(0x0029, 0x0011); // Set VCM[5:0] for VCOMH
		LCD_WriteReg(0x002B, 0x000B); // Set Frame Rate
		ili9320_Delay(50); // Delay 50ms
		LCD_WriteReg(0x0020, 0x0000); // GRAM horizontal Address
		LCD_WriteReg(0x0021, 0x0000); // GRAM Vertical Address
		// ----------- Adjust the Gamma Curve ----------//
		LCD_WriteReg(0x0030, 0x0000);
		LCD_WriteReg(0x0031, 0x0106);
		LCD_WriteReg(0x0032, 0x0000);
		LCD_WriteReg(0x0035, 0x0204);
		LCD_WriteReg(0x0036, 0x160A);
		LCD_WriteReg(0x0037, 0x0707);
		LCD_WriteReg(0x0038, 0x0106);
		LCD_WriteReg(0x0039, 0x0707);
		LCD_WriteReg(0x003C, 0x0402);
		LCD_WriteReg(0x003D, 0x0C0F);
		//------------------ Set GRAM area ---------------//
		LCD_WriteReg(0x0050, 0x0000); // Horizontal GRAM Start Address
		LCD_WriteReg(0x0051, 0x00EF); // Horizontal GRAM End Address
		LCD_WriteReg(0x0052, 0x0000); // Vertical GRAM Start Address
		LCD_WriteReg(0x0053, 0x013F); // Vertical GRAM Start Address
		LCD_WriteReg(0x0060, 0x2700); // Gate Scan Line
		LCD_WriteReg(0x0061, 0x0001); // NDL,VLE, REV
		LCD_WriteReg(0x006A, 0x0000); // set scrolling line
		//-------------- Partial Display Control ---------//
		LCD_WriteReg(0x0080, 0x0000);
		LCD_WriteReg(0x0081, 0x0000);
		LCD_WriteReg(0x0082, 0x0000);
		LCD_WriteReg(0x0083, 0x0000);
		LCD_WriteReg(0x0084, 0x0000);
		LCD_WriteReg(0x0085, 0x0000);
		//-------------- Panel Control -------------------//
		LCD_WriteReg(0x0090, 0x0010);
		LCD_WriteReg(0x0092, 0x0600);
		LCD_WriteReg(0x0007,0x0021);
		ili9320_Delay(50);
		LCD_WriteReg(0x0007,0x0061);
		ili9320_Delay(50);
		LCD_WriteReg(0x0007,0x0133);  // 262K color and display ON
		ili9320_Delay(50);
	}
	else if(DeviceCode==0x9919)
	{
		//*********POWER ON &RESET DISPLAY OFF
		 LCD_WriteReg(0x28,0x0006);

		 LCD_WriteReg(0x00,0x0001);

		 LCD_WriteReg(0x10,0x0000);

		 LCD_WriteReg(0x01,0x72ef);

		 LCD_WriteReg(0x02,0x0600);

		 LCD_WriteReg(0x03,0x6a38);

		 LCD_WriteReg(0x11,0x6874);//70


		     //  RAM WRITE DATA MASK
		 LCD_WriteReg(0x0f,0x0000);
		    //  RAM WRITE DATA MASK
		 LCD_WriteReg(0x0b,0x5308);

		 LCD_WriteReg(0x0c,0x0003);

		 LCD_WriteReg(0x0d,0x000a);

		 LCD_WriteReg(0x0e,0x2e00);  //0030

		 LCD_WriteReg(0x1e,0x00be);

		 LCD_WriteReg(0x25,0x8000);

		 LCD_WriteReg(0x26,0x7800);

		 LCD_WriteReg(0x27,0x0078);

		 LCD_WriteReg(0x4e,0x0000);

		 LCD_WriteReg(0x4f,0x0000);

		 LCD_WriteReg(0x12,0x08d9);

		 // -----------------Adjust the Gamma Curve----//
		 LCD_WriteReg(0x30,0x0000);	 //0007

		 LCD_WriteReg(0x31,0x0104);	   //0203

		 LCD_WriteReg(0x32,0x0100);		//0001

		 LCD_WriteReg(0x33,0x0305);	  //0007

		 LCD_WriteReg(0x34,0x0505);	  //0007

		 LCD_WriteReg(0x35,0x0305);		 //0407

		 LCD_WriteReg(0x36,0x0707);		 //0407

		 LCD_WriteReg(0x37,0x0300);		  //0607

		 LCD_WriteReg(0x3a,0x1200);		 //0106

		 LCD_WriteReg(0x3b,0x0800);

		 LCD_WriteReg(0x07,0x0033);
	}
	else if(DeviceCode==0x1505)
	{
// second release on 3/5  ,luminance is acceptable,water wave appear during camera preview
        LCD_WriteReg(0x0007,0x0000);
        ili9320_Delay(5);
        LCD_WriteReg(0x0012,0x011C);//0x011A   why need to set several times?
        LCD_WriteReg(0x00A4,0x0001);//NVM
    //
        LCD_WriteReg(0x0008,0x000F);
        LCD_WriteReg(0x000A,0x0008);
        LCD_WriteReg(0x000D,0x0008);

  //GAMMA CONTROL/
        LCD_WriteReg(0x0030,0x0707);
        LCD_WriteReg(0x0031,0x0007); //0x0707
        LCD_WriteReg(0x0032,0x0603);
        LCD_WriteReg(0x0033,0x0700);
        LCD_WriteReg(0x0034,0x0202);
        LCD_WriteReg(0x0035,0x0002); //?0x0606
        LCD_WriteReg(0x0036,0x1F0F);
        LCD_WriteReg(0x0037,0x0707); //0x0f0f  0x0105
        LCD_WriteReg(0x0038,0x0000);
        LCD_WriteReg(0x0039,0x0000);
        LCD_WriteReg(0x003A,0x0707);
        LCD_WriteReg(0x003B,0x0000); //0x0303
        LCD_WriteReg(0x003C,0x0007); //?0x0707
        LCD_WriteReg(0x003D,0x0000); //0x1313//0x1f08
        ili9320_Delay(5);
        LCD_WriteReg(0x0007,0x0001);
        LCD_WriteReg(0x0017,0x0001);   //Power supply startup enable
        ili9320_Delay(5);

  //power control//
        LCD_WriteReg(0x0010,0x17A0);
        LCD_WriteReg(0x0011,0x0217); //reference voltage VC[2:0]   Vciout = 1.00*Vcivl
        LCD_WriteReg(0x0012,0x011E);//0x011c  //Vreg1out = Vcilvl*1.80   is it the same as Vgama1out ?
        LCD_WriteReg(0x0013,0x0F00); //VDV[4:0]-->VCOM Amplitude VcomL = VcomH - Vcom Ampl
        LCD_WriteReg(0x002A,0x0000);
        LCD_WriteReg(0x0029,0x000A); //0x0001F  Vcomh = VCM1[4:0]*Vreg1out    gate source voltage??
        LCD_WriteReg(0x0012,0x013E); // 0x013C  power supply on
           //Coordinates Control//
        LCD_WriteReg(0x0050,0x0000);//0x0e00
        LCD_WriteReg(0x0051,0x00EF);
        LCD_WriteReg(0x0052,0x0000);
        LCD_WriteReg(0x0053,0x013F);
    //Pannel Image Control//
        LCD_WriteReg(0x0060,0x2700);
        LCD_WriteReg(0x0061,0x0001);
        LCD_WriteReg(0x006A,0x0000);
        LCD_WriteReg(0x0080,0x0000);
    //Partial Image Control//
        LCD_WriteReg(0x0081,0x0000);
        LCD_WriteReg(0x0082,0x0000);
        LCD_WriteReg(0x0083,0x0000);
        LCD_WriteReg(0x0084,0x0000);
        LCD_WriteReg(0x0085,0x0000);
  //Panel Interface Control//
        LCD_WriteReg(0x0090,0x0013); //0x0010 frenqucy
        LCD_WriteReg(0x0092,0x0300);
        LCD_WriteReg(0x0093,0x0005);
        LCD_WriteReg(0x0095,0x0000);
        LCD_WriteReg(0x0097,0x0000);
        LCD_WriteReg(0x0098,0x0000);

        LCD_WriteReg(0x0001,0x0100);
        LCD_WriteReg(0x0002,0x0700);
        LCD_WriteReg(0x0003,0x1030);
        LCD_WriteReg(0x0004,0x0000);
        LCD_WriteReg(0x000C,0x0000);
        LCD_WriteReg(0x000F,0x0000);
        LCD_WriteReg(0x0020,0x0000);
        LCD_WriteReg(0x0021,0x0000);
        LCD_WriteReg(0x0007,0x0021);
        ili9320_Delay(20);
        LCD_WriteReg(0x0007,0x0061);
        ili9320_Delay(20);
        LCD_WriteReg(0x0007,0x0173);
        ili9320_Delay(20);
	}
	else if(DeviceCode==0x8989)
	{
		LCD_WriteReg(0x0000,0x0001);    ili9320_Delay(50000);  //打开晶振
    	LCD_WriteReg(0x0003,0xA8A4);    ili9320_Delay(50000);   //0xA8A4
    	LCD_WriteReg(0x000C,0x0000);    ili9320_Delay(50000);
    	LCD_WriteReg(0x000D,0x080C);    ili9320_Delay(50000);
    	LCD_WriteReg(0x000E,0x2B00);    ili9320_Delay(50000);
    	LCD_WriteReg(0x001E,0x00B0);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0001,0x2B3F);    ili9320_Delay(50000);   //驱动输出控制320*240  0x6B3F
    	LCD_WriteReg(0x0002,0x0600);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0010,0x0000);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0011,0x6070);    ili9320_Delay(50000);        //0x4030           //定义数据格式  16位色 		横屏 0x6058
    	LCD_WriteReg(0x0005,0x0000);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0006,0x0000);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0016,0xEF1C);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0017,0x0003);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0007,0x0233);    ili9320_Delay(50000);        //0x0233
    	LCD_WriteReg(0x000B,0x0000);    ili9320_Delay(50000);
    	LCD_WriteReg(0x000F,0x0000);    ili9320_Delay(50000);        //扫描开始地址
    	LCD_WriteReg(0x0041,0x0000);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0042,0x0000);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0048,0x0000);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0049,0x013F);    ili9320_Delay(50000);
    	LCD_WriteReg(0x004A,0x0000);    ili9320_Delay(50000);
    	LCD_WriteReg(0x004B,0x0000);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0044,0xEF00);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0045,0x0000);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0046,0x013F);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0030,0x0707);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0031,0x0204);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0032,0x0204);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0033,0x0502);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0034,0x0507);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0035,0x0204);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0036,0x0204);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0037,0x0502);    ili9320_Delay(50000);
    	LCD_WriteReg(0x003A,0x0302);    ili9320_Delay(50000);
    	LCD_WriteReg(0x003B,0x0302);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0023,0x0000);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0024,0x0000);    ili9320_Delay(50000);
    	LCD_WriteReg(0x0025,0x8000);    ili9320_Delay(50000);
    	LCD_WriteReg(0x004f,0);        //行首址0
    	LCD_WriteReg(0x004e,0);        //列首址0
	}
  for(i=50000;i>0;i--);
}

/****************************************************************************
* 名    称：void ili9320_SetCursor(u16 x,u16 y)
* 功    能：设置屏幕座标
* 入口参数：x      行座标
*           y      列座标
* 出口参数：无
* 说    明：
* 调用方法：ili9320_SetCursor(10,10);
****************************************************************************/
__inline void ili9320_SetCursor(u16 x,u16 y)
{
	if (DeviceCode == 0x5420)
	{
		LCD_WriteReg(0x0200, y);//StartY;
		LCD_WriteReg(0x0201, 399 - x); //StartX);
	}
	else if(DeviceCode==0x8989)
	{
	 	LCD_WriteReg(0x004e,y);        //行
    	LCD_WriteReg(0x004f,0x13f-x);  //列
	}
	else if(DeviceCode==0x9919)
	{
		LCD_WriteReg(0x004e,x); // 行
  		LCD_WriteReg(0x004f,y); // 列
	}
	else
	{
  		LCD_WriteReg(0x0020,y); // 行
  		LCD_WriteReg(0x0021,0x13f-x); // 列
	}
}

/****************************************************************************
* 名    称：void ili9320_SetWindows(u16 StartX,u16 StartY,u16 EndX,u16 EndY)
* 功    能：设置窗口区域
* 入口参数：StartX     行起始座标
*           StartY     列起始座标
*           EndX       行结束座标
*           EndY       列结束座标
* 出口参数：无
* 说    明：
* 调用方法：ili9320_SetWindows(0,0,100,100)；
****************************************************************************/
__inline void ili9320_SetWindows(u16 StartX,u16 StartY,u16 EndX,u16 EndY)
{
	if (DeviceCode == 0x5420)
	{
		ili9320_SetCursor(StartX,StartY);
		//LCD_WriteReg(0x0210, StartX);
		//LCD_WriteReg(0x0212, StartY);
		//LCD_WriteReg(0x0211, EndX);
		//LCD_WriteReg(0x0213, EndY);

		LCD_WriteReg(0x0212, StartX);
		LCD_WriteReg(0x0210, StartY);
		LCD_WriteReg(0x0213, EndX);
		LCD_WriteReg(0x0212, EndY);

	}
	else
	{
		ili9320_SetCursor(StartX,StartY);
		LCD_WriteReg(0x0050, StartX);
		LCD_WriteReg(0x0052, StartY);
		LCD_WriteReg(0x0051, EndX);
		LCD_WriteReg(0x0053, EndY);
	}
}

/****************************************************************************
* 名    称：void ili9320_Clear(u16 dat)
* 功    能：将屏幕填充成指定的颜色，如清屏，则填充 0xffff
* 入口参数：dat      填充值
* 出口参数：无
* 说    明：
* 调用方法：ili9320_Clear(0xffff);
****************************************************************************/
void ili9320_Clear(u16 Color)
{
	u32 index=0;
	u32 size;

	ili9320_SetCursor(0,0);
	LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

	if (DeviceCode == 0x5420)
	{
		size = 400 * 240;
	}
	else
	{
		size = 76800;
	}
	for(index=0;index<size;index++)
	{
		LCD->LCD_RAM=Color;
	}
}

/****************************************************************************
* 名    称：u16 ili9320_GetPoint(u16 x,u16 y)
* 功    能：获取指定座标的颜色值
* 入口参数：x      行座标
*           y      列座标
* 出口参数：当前座标颜色值
* 说    明：
* 调用方法：i=ili9320_GetPoint(10,10);
****************************************************************************/
u16 ili9320_GetPoint(u16 x,u16 y)
{
	ili9320_SetCursor(x,y);

	if (DeviceCode == 0x5420)
	{
		if (g_Is4001 == 1)
		{
			return LCD_ReadRAM();
		}
		else
		{
			return (ili9320_BGR2RGB(LCD_ReadRAM()));
		}		
	}
	return (ili9320_BGR2RGB(LCD_ReadRAM()));
}
/****************************************************************************
* 名    称：void ili9320_SetPoint(u16 x,u16 y,u16 point)
* 功    能：在指定座标画点
* 入口参数：x      行座标
*           y      列座标
*           point  点的颜色
* 出口参数：无
* 说    明：
* 调用方法：ili9320_SetPoint(10,10,0x0fe0);
****************************************************************************/
void ili9320_SetPoint(u16 x,u16 y,u16 point)
{
	if (DeviceCode == 0x5420)
	{
		 if ((x>400) || (y>240) )
		 	return;
	}
	else
	{
		 if ((x>320) || (y>240) )
		 	return;
	}

  ili9320_SetCursor(x,y);

  LCD_WriteRAM_Prepare();
  LCD_WriteRAM(point);
}

/****************************************************************************
* 名    称：void ili9320_DrawPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *pic)
* 功    能：在指定座标范围显示一副图片
* 入口参数：StartX     行起始座标
*           StartY     列起始座标
*           EndX       行结束座标
*           EndY       列结束座标
            pic        图片头指针
* 出口参数：无
* 说    明：图片取模格式为水平扫描，16位颜色模式
* 调用方法：ili9320_DrawPicture(0,0,100,100,(u16*)demo);
****************************************************************************/
void ili9320_DrawPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *pic)
{
  u32  i;

  ili9320_SetWindows(StartX,StartY,EndX,EndY);
  ili9320_SetCursor(StartX,StartY);

  LCD_WriteRAM_Prepare();
  for (i=0;i<(EndX*EndY);i++)
  {
      LCD_WriteRAM(*pic++);
  }
}

/****************************************************************************
* 名    称：void ili9320_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor)
* 功    能：在指定座标显示一个8x16点阵的ascii字符
* 入口参数：x          行座标
*           y          列座标
*           charColor  字符的颜色
*           bkColor    字符背景颜色
* 出口参数：无
* 说    明：显示范围限定为可显示的ascii码
* 调用方法：ili9320_PutChar(10,10,'a',0x0000,0xffff);
****************************************************************************/
void ili9320_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor)
{
  u16 i=0;
  u16 j=0;

  u8 tmp_char=0;

  for (i=0;i<16;i++)
  {
    tmp_char=ascii_8x16[((c-0x20)*16)+i];
    for (j=0;j<8;j++)
    {
      if ( (tmp_char >> 7-j) & 0x01 == 0x01)
        {
          ili9320_SetPoint(x+j,y+i,charColor); // 字符颜色
        }
        else
        {
          ili9320_SetPoint(x+j,y+i,bkColor); // 背景颜色
        }
    }
  }
}

/****************************************************************************
* 名    称：u16 ili9320_BGR2RGB(u16 c)
* 功    能：RRRRRGGGGGGBBBBB 改为 BBBBBGGGGGGRRRRR 格式
* 入口参数：c      BRG 颜色值
* 出口参数：RGB 颜色值
* 说    明：内部函数调用
* 调用方法：
****************************************************************************/
u16 ili9320_BGR2RGB(u16 c)
{
  u16  r, g, b, rgb;

  b = (c>>0)  & 0x1f;
  g = (c>>5)  & 0x3f;
  r = (c>>11) & 0x1f;

  rgb =  (b<<11) + (g<<5) + (r<<0);

  return( rgb );
}

/****************************************************************************
* 名    称：void ili9320_BackLight(u8 status)
* 功    能：开、关液晶背光
* 入口参数：status     1:背光开  0:背光关
* 出口参数：无
* 说    明：
* 调用方法：ili9320_BackLight(1);
****************************************************************************/
void ili9320_BackLight(u8 status)
{
  if ( status >= 1 )
  {
    Lcd_Light_ON;
  }
  else
  {
    Lcd_Light_OFF;
  }
}

/****************************************************************************
* 名    称：void ili9320_Delay(vu32 nCount)
* 功    能：延时
* 入口参数：nCount   延时值
* 出口参数：无
* 说    明：
* 调用方法：ili9320_Delay(10000);
****************************************************************************/
void ili9320_Delay(vu32 nCount)
{
  for(; nCount != 0; nCount--);
}
