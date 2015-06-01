#include "includes.h"
#include "SetPWM.h"


/*
*********************************************************************************************************
*	函 数 名: SetPWM
*	功能说明: 初始化GPIO,配置为PWM模式
*	形    参：_bright 亮度，0是灭，255是最亮
*	返 回 值: 无
*********************************************************************************************************
*/
void SetPWM(unsigned short _bright,unsigned int PWM_CH)
{
	/*
		背光口线是 PB1, 复用功能选择 TIM3_CH4
	
		当关闭背光时，将CPU IO设置为浮动输入模式（推荐设置为推挽输出，并驱动到低电平)
		将TIM3关闭以节约功耗
	*/

	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* 第1步：打开GPIOB RCC_APB2Periph_AFIO 的时钟	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	if(PWM_BACK_LIGHT == PWM_CH)
	{		
			if (_bright == 0)
			{
				/* 配置背光GPIO为输入模式 */
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_Init(GPIOB, &GPIO_InitStructure);

				/* 关闭TIM3 */
				TIM_Cmd(TIM3, DISABLE);
				return;
			}
			else if (_bright == BRIGHT_MAX)	/* 最大亮度 */
			{
				/* 配置背光GPIO为推挽输出模式 */
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_Init(GPIOB, &GPIO_InitStructure);

				GPIO_SetBits(GPIOB, GPIO_Pin_1);

				/* 关闭TIM3 */
				TIM_Cmd(TIM3, DISABLE);
				return;
			}
	}
	
	/* Configure the PIN */
	{
		{
			/* 配置背光GPIO为复用推挽输出模式 */
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOB, &GPIO_InitStructure);

			/* 使能TIM3的时钟 */
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		}
		
		{
			/* 配置背光GPIO为复用推挽输出模式 */
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);

			/* 使能TIM3的时钟 */
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		}
	}
	
	{
		/*
			TIM3 配置: 产生2路PWM信号;
			TIM3CLK = 72 MHz, Prescaler = 0(不分频), TIM3 counter clock = 72 MHz
			计算公式：
			PWM输出频率 = TIM3 counter clock /(ARR + 1)

			我们期望设置为100Hz

			如果不对TIM3CLK预分频，那么不可能得到100Hz低频。
			我们设置分频比 = 1000， 那么  TIM3 counter clock = 72KHz
			TIM_Period = 720 - 1;
			频率下不来。
		 */
		TIM_TimeBaseStructure.TIM_Period = 720 - 1;	/* TIM_Period = TIM3 ARR Register */
		TIM_TimeBaseStructure.TIM_Prescaler = 0;
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

		/* PWM1 Mode configuration */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;// 输出模式
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	}	

	/*
		_bright = 1 时, TIM_Pulse = 1
		_bright = 255 时, TIM_Pulse = TIM_Period
	*/
	
	if(PWM_BACK_LIGHT == PWM_CH)
	{
		TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period * _bright) / BRIGHT_MAX;	/* 改变占空比 */

		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OC4Init(TIM3, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	}
	
	if(PWM_PB0_CH3 == PWM_CH)
	{
		TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period * _bright) / BRIGHT_MAX;	/* 改变占空比 */

		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OC3Init(TIM3, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	}
	
	
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);

	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}


/*
*********************************************************************************************************
*	函 数 名: SetPWM_TIM4
*	功能说明: 初始化GPIO,配置为PWM模式
*	形    参：_bright 亮度，0是灭，255是最亮
*	返 回 值: 无
*********************************************************************************************************
*/
void SetPWM_TIM4(unsigned short _bright,unsigned int PWM_CH)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* 第1步：打开GPIOB RCC_APB2Periph_AFIO 的时钟	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);


	
	/* Configure the PIN */
	{
		/* 配置背光GPIO为复用推挽输出模式 */
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* 使能TIM3的时钟 */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	}
		
			
	{
		/*
			TIM4 配置: 产生2路PWM信号;
			TIM3CLK = 72 MHz, Prescaler = 0(不分频), TIM3 counter clock = 72 MHz
			计算公式：
			PWM输出频率 = TIM3 counter clock /(ARR + 1)

			我们期望设置为100Hz

			如果不对TIM3CLK预分频，那么不可能得到100Hz低频。
			我们设置分频比 = 1000， 那么  TIM3 counter clock = 72KHz
			TIM_Period = 720 - 1;
			频率下不来。
		 */
		TIM_TimeBaseStructure.TIM_Period = 720 - 1;	/* TIM_Period = TIM3 ARR Register */
		TIM_TimeBaseStructure.TIM_Prescaler = 0;
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

		/* PWM1 Mode configuration */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;// 输出模式
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	}	

	/*
		_bright = 1 时, TIM_Pulse = 1
		_bright = 255 时, TIM_Pulse = TIM_Period
	*/
	

	
	if(PWM_PB8_CH3 == PWM_CH)
	{
		TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period * _bright) / BRIGHT_MAX;	/* 改变占空比 */

		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OC3Init(TIM4, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	}
	
	if(PWM_PB9_CH4 == PWM_CH)
	{
		TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period * _bright) / BRIGHT_MAX;	/* 改变占空比 */

		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OC4Init(TIM4, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	}
	
	TIM_ARRPreloadConfig(TIM4, ENABLE);

	/* TIM4 enable counter */
	TIM_Cmd(TIM4, ENABLE);
}
