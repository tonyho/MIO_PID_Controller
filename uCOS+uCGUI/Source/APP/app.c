/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
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
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-EVAL Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : Brian Nagel
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>

#include "GUI.h"
#include "GRAPH.h"
#include "EDIT.h"


/*
*********************************************************************************************************
*                                      LOCAL TASK FUNCTION PROTOTYPES
*********************************************************************************************************
*/
/* ----------------- APPLICATION GLOBALS ------------------ */
static OS_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
static OS_STK AppTaskUserIFStk[APP_TASK_USER_IF_STK_SIZE];
static OS_STK AppTaskKbdStk[APP_TASK_KBD_STK_SIZE];
static OS_STK ADTaskStk[APP_TASK_AD_STK_SIZE];
static OS_STK AppPIDGraphSTK[APP_TASK_PID_GRAPH_STK_SIZE];


/*
*********************************************************************************************************
*                                      LOCAL Variable PROTOTYPES
*********************************************************************************************************
*/

u16 usValue; //!< Store the AD value
u16 usValue2; //!< Store the AD value
u16 PID_Out[3]; //!< Store the PID value to be output 3PWM Output


/*
*********************************************************************************************************
*                                      Extern Variable 
*********************************************************************************************************
*/

extern GRAPH_DATA_Handle  hData,hData2;
extern int PID_CFG_Flag;
extern EDIT_Handle   Edit_PWM1,Edit_PWM2,Edit_PWM3;	
extern PID value_PID[6];
extern EDIT_Handle Edit_AD1,Edit_AD2;
extern int ClosePWM;




/*
*********************************************************************************************************
*                                      Extern Funtion  PROTOTYPES
*********************************************************************************************************
*/

void MainTask(void);


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void AppTaskCreate(void);
static void AppTaskStart(void *p_arg);
static void AppTaskUserIF(void *p_arg);
static void AppTaskKbd(void *p_arg);
static void AppTaskAD(void *p_arg);
static void AppTaskPIDGraph(void *p_arg);


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/


int  main (void)
{
  INT8U  err;
	
	/* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);  

	
	ADC_Configuration(ADC1_PA2_Channel2);
	ADC2_Configuration(ADC2_PA0_Channel0);	
	
//	USART_Config(USART1,115200);     //初始化串口1
 // USART_OUT(USART1,"BeforeOS");    	  //向串口1发送开机字符。
	
 

  OSInit();                          /* Initialize "uC/OS-II, The Real-Time Kernel"              */

   /* Create the start task */
	OSTaskCreateExt(AppTaskStart,(void *)0,(OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE-1],APP_TASK_START_PRIO,APP_TASK_START_PRIO,(OS_STK *)&AppTaskStartStk[0],APP_TASK_START_STK_SIZE,(void *)0,OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);

#if (OS_TASK_NAME_SIZE > 13)
    OSTaskNameSet(APP_TASK_START_PRIO, "Start Task", &err);
#endif

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)       */
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
  (void)p_arg;

   BSP_Init();                                                 /* Initialize BSP functions                                 */
   #if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity                                   */
   #endif

   AppTaskCreate();                                            /* Create application tasks                                 */
    
   while(DEF_TRUE)
	 {  
	  /* Task body, always written as an infinite loop. */
	   OSTaskSuspend(OS_PRIO_SELF);	  
   }
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION TASKS
*
* Description:  This function creates the application tasks.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppTaskCreate(void)
{
  INT8U  err;
	
	//!< Use IF
  OSTaskCreateExt(AppTaskUserIF,(void *)0,(OS_STK *)&AppTaskUserIFStk[APP_TASK_USER_IF_STK_SIZE-1],APP_TASK_USER_IF_PRIO,APP_TASK_USER_IF_PRIO,(OS_STK *)&AppTaskUserIFStk[0],
                    APP_TASK_USER_IF_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);
	
	//!< 增加一路PID 和 Graph的AddValue 
	OSTaskCreateExt(AppTaskPIDGraph,(void *)0,(OS_STK *)&AppPIDGraphSTK[APP_TASK_PID_GRAPH_STK_SIZE-1],APP_TASK_PID_GRAPH_PRIO,APP_TASK_PID_GRAPH_PRIO,(OS_STK *)&AppPIDGraphSTK[0],
                    APP_TASK_PID_GRAPH_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);

	//!< AD 采样
	OSTaskCreateExt(AppTaskAD,(void *)0,    (OS_STK *)&ADTaskStk[APP_TASK_AD_STK_SIZE-1],           APP_TASK_AD_PRIO,APP_TASK_AD_PRIO,           (OS_STK *)&ADTaskStk[0],
                    APP_TASK_AD_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);
	
#if (OS_TASK_NAME_SIZE > 8)
  OSTaskNameSet(APP_TASK_USER_IF_PRIO, "User I/F", &err);
#endif

	//!< Touch Pad
  OSTaskCreateExt(AppTaskKbd,(void *)0,(OS_STK *)&AppTaskKbdStk[APP_TASK_KBD_STK_SIZE-1],APP_TASK_KBD_PRIO,APP_TASK_KBD_PRIO,(OS_STK *)&AppTaskKbdStk[0],
                    APP_TASK_KBD_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);

#if (OS_TASK_NAME_SIZE > 8)
  OSTaskNameSet(APP_TASK_KBD_PRIO, "Keyboard", &err);
#endif
}


/*
*********************************************************************************************************
*                                         USER INTERFACE TASK
*
* Description : This task updates the LCD screen based on messages passed to it by AppTaskKbd().
*
* Arguments   : p_arg   is the argument passed to 'AppStartUserIF()' by 'OSTaskCreate()'.
*
* Returns     : none
*********************************************************************************************************
*/

static  void  AppTaskUserIF (void *p_arg)
{
 
 (void)p_arg;
  // GUI_Init();
  while(DEF_TRUE) 
    {
			MainTask(); 
    }
}


/*
*********************************************************************************************************
*                                    KEYBOARD RESPONSE TASK
*
* Description : This task monitors the state of the push buttons and passes messages to AppTaskUserIF()
*
* Arguments   : p_arg   is the argument passed to 'AppStartKbd()' by 'OSTaskCreate()'.
*
* Returns     : none
*********************************************************************************************************
*/

static  void  AppTaskKbd (void *p_arg)
{
   u8 tick=0;
  (void)p_arg;
   
	 while(DEF_TRUE) 
   {
			tick++;
      OSTimeDlyHMSM(0,0,0,10); 
	    GUI_TOUCH_Exec(); 
   }
}

/*
*********************************************************************************************************
*                                    AD RESPONSE TASK
*
* Description : This task monitors the AD and Get the AD Value
*
* Arguments   : p_arg   is the argument passed to 'AppStartKbd()' by 'OSTaskCreate()'.
*
* Returns     : none
*********************************************************************************************************
*/
static void AppTaskAD(void *p_arg)
{
	 u8 tick=0;

  (void)p_arg;
	
	
	
	while(1)
	{		
			tick++;
		
			if(1==tick)
			{
				  GPIO_SetBits(GPIOF,GPIO_Pin_6);				
					GPIO_ResetBits(GPIOF,GPIO_Pin_10);
			
			}
			else
			{
					GPIO_ResetBits(GPIOF,GPIO_Pin_6);								
					GPIO_SetBits(GPIOF,GPIO_Pin_10);
					
					tick = 0;
				
					AdcPro(); // AD Process
					Adc2Pro();
			}
			
			usValue = GetADC();
			usValue2 = GetADC2();

#ifdef EDIT_AD				
			EDIT_SetValue(Edit_AD1,usValue  * 0.806);//825/1024);  //换成温度
			EDIT_SetValue(Edit_AD2,usValue2 * 0.806);
#endif
			
			//  USART_OUT(USART1,"Task"); 
			//USART_SendData(USART1,'a');
			//while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
			OSTimeDlyHMSM(0,0,0,500);			
	}
	
}

/*
*********************************************************************************************************
*                                    PID GraphReflah RESPONSE TASK
*
* Description : This task monitors the AD and Get the AD Value
*
* Arguments   : p_arg  
*
* Returns     : none
*********************************************************************************************************
*/
static void AppTaskPIDGraph(void *p_arg)
{
	u8 tick=0;

  (void)p_arg;
		
	while(1)
	{
			tick++;
		
			if(1==tick)
			{				
				GPIO_ResetBits(GPIOF,GPIO_Pin_7);		
			}
			else
			{
				GPIO_SetBits(GPIOF,GPIO_Pin_7);
				
				tick = 0;										
			}
		
			if(0 == PID_CFG_Flag)
			{
				/* PID Calculat */
				{
					PID_Out[0] =  PIDCalc(value_PID[0].SetPoint,usValue/12.4,0) // usValue应该除以的系数不确定??
											+ PIDCalc(value_PID[1].SetPoint,usValue2/12.4,1); 								
					
					PID_Out[1] =  PIDCalc(value_PID[2].SetPoint,usValue/12.4,2)
											+ PIDCalc(value_PID[3].SetPoint,usValue2/12.4,3);
					
					PID_Out[2] =  PIDCalc(value_PID[4].SetPoint,usValue/12.4,4)
											+ PIDCalc(value_PID[5].SetPoint,usValue2/12.4,5);
				}

#define PWM_OUT_ALL
#ifdef PWM_OUT_ALL				
				if(0 == ClosePWM)
				/* PWM Output */
				{
					SetPWM(PID_Out[0], PWM_PB0_CH3);			
					SetPWM_TIM4(PID_Out[1], PWM_PB8_CH3);
					SetPWM_TIM4(PID_Out[2],PWM_PB9_CH4);
				}
				else
				{
					SetPWM(0, PWM_PB0_CH3);			
					SetPWM_TIM4(0, PWM_PB8_CH3);
					SetPWM_TIM4(0,PWM_PB9_CH4);
				}
#endif				
			
				EDIT_SetValue(Edit_PWM1,PID_Out[0]);
				EDIT_SetValue(Edit_PWM2,PID_Out[1]);
				EDIT_SetValue(Edit_PWM3,PID_Out[2]);
				

			//1V对应100摄氏度 1V对应1241 因为坐标缩小了一倍 故1241/100/2 = 6.2	
			GRAPH_DATA_YT_AddValue(hData, usValue/6.2);  //Red Graph -- PA0 3LongBig Wire __Good 
			GRAPH_DATA_YT_AddValue(hData2,usValue2/6.2); // Blue Graph --PA2 3White Wire _Shake
			}
											
			OSTimeDlyHMSM(0,0,0,500);
	}
}
