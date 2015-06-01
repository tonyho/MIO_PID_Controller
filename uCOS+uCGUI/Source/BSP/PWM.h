#ifndef __PWM__
#define __PWM__

/*******************************************************************************
* Function Name  : None
* Description    : 定时器3 PWM输出占空比全局变量 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

#define CH1 1
#define CH2 2
#define CH3 3
#define CH4 4

/*******************************************************************************
* Function Name  : TIM3PWM_4Channel_Configuration
* Description    : 初始化定时器3，4个通道的PWM输出，占空比初始化为50% 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3PWM_4Channel_Configuration(void);

/*******************************************************************************
* Function Name  : Adjust_DutyCycle
* Description    : 定时器3调节4个通道的占空比 
* Input          : CHx : 输出通道   DC : 调节到多少量
* Output         : None
* Return         : None
*******************************************************************************/
void Adjust_TIM3DutyCycle(unsigned char CHx , unsigned int DC);

#endif

