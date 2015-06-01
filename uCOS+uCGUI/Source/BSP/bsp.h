/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-EVAL Evaluation Board
*
* Filename      : bsp.h
* Version       : V1.00
* Programmer(s) : Brian Nagel
*********************************************************************************************************
*/

#ifndef  __BSP_H__
#define  __BSP_H__

/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_GLOBALS
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif

#define TP_CS()  GPIO_ResetBits(GPIOG,GPIO_Pin_11)
#define TP_DCS() GPIO_SetBits(GPIOG,GPIO_Pin_11)
/*
#define SetSCK() GPIO_SetBits(GPIOA,GPIO_Pin_5) 
#define ClearSCK() GPIO_ResetBits(GPIOA,GPIO_Pin_5) 
#define SetMOSI()  GPIO_SetBits(GPIOA,GPIO_Pin_7)
#define ClearMOSI() GPIO_ResetBits(GPIOA,GPIO_Pin_7)
#define ReadMISO() GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)*/
#define WaitTPReady() while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==0)

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/




/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/
#define uint8_t unsigned char

void BSP_Init(void);
void BSP_IntDisAll(void);
u16 TPReadX(void);
u16 TPReadY(void);
void USART_Config(USART_TypeDef* USARTx,u32 baud);
char *itoa(int value, char *string, int radix);
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);


#endif                                                          /* End of module include.                               */
