#include "includes.h"

u16 g_usAdcValue;	/* ADC 采样值的平均值 */
u16 g_usAdc2Value;

/*
*********************************************************************************************************
*	函 数 名: ADC_Configuration
*	功能说明: 配置ADC, PC4模式
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
 void ADC_Configuration(unsigned int Channel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	__IO u16 ADCConvertedValue; //!< 

	if(Channel == ADC1_PC4_Channel1)
	{
		/* 使能 ADC1 and GPIOC clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);

		/* 配置PC4为模拟输入(ADC Channel14) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		/* 配置ADC1, 不用DMA, 用软件触发 */
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = ENABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfChannel = 1;
		ADC_Init(ADC1, &ADC_InitStructure);

		/* 配置ADC1 规则通道14 channel14 configuration */
		ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_55Cycles5);
	}
	
	if(Channel == ADC1_PA2_Channel2)
	{
		/* 使能 ADC1 and GPIOC clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

		/* 配置PC4为模拟输入(ADC Channel14) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* 配置ADC1, 不用DMA, 用软件触发 */
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = ENABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfChannel = 1;
		ADC_Init(ADC1, &ADC_InitStructure);

		/* 配置ADC1 规则通道2 channel2 configuration */
		ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_55Cycles5);
	}
	
	if(Channel == ADC1_PA0_Channel0)
	{
		/* 使能 ADC1 and GPIOC clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

		/* 配置PA0为模拟输入(ADC Channel0) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* 配置ADC1, 不用DMA, 用软件触发 */
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = ENABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		
		ADC_InitStructure.ADC_NbrOfChannel = 1;
		
		ADC_Init(ADC1, &ADC_InitStructure);

		/* 配置ADC1 规则通道2 channel2 configuration */
		ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	}
	
	/* 使能ADC1 DMA功能 */
	ADC_DMACmd(ADC1, ENABLE);

	/* 使能 ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* 使能ADC1 复位校准寄存器 */
	ADC_ResetCalibration(ADC1);
	/* 检查ADC1的复位寄存器 */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* 启动ADC1校准 */
	ADC_StartCalibration(ADC1);
	/* 检查校准是否结束 */
	while(ADC_GetCalibrationStatus(ADC1));

	/* 软件启动ADC转换 */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: ADC2_Configuration
*	功能说明: 配置ADC, PC4模式
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
 void ADC2_Configuration(unsigned int Channel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	__IO u16 ADCConvertedValue; //!< 

	if(Channel == ADC2_PC4_Channel1)
	{
		/* 使能 ADC1 and GPIOC clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2 | RCC_APB2Periph_GPIOC, ENABLE);

		/* 配置PC4为模拟输入(ADC Channel14) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		/* 配置ADC1, 不用DMA, 用软件触发 */
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = ENABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfChannel = 1;
		ADC_Init(ADC2, &ADC_InitStructure);

		/* 配置ADC1 规则通道14 channel14 configuration */
		ADC_RegularChannelConfig(ADC2, ADC_Channel_14, 1, ADC_SampleTime_55Cycles5);
	}
	
	if(Channel == ADC2_PA2_Channel2)
	{
		/* 使能 ADC1 and GPIOC clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2 | RCC_APB2Periph_GPIOA, ENABLE);

		/* 配置PC4为模拟输入(ADC Channel14) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* 配置ADC1, 不用DMA, 用软件触发 */
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = ENABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfChannel = 1;
		ADC_Init(ADC2, &ADC_InitStructure);

		/* 配置ADC1 规则通道2 channel2 configuration */
		ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_55Cycles5);
	}
	
	if(Channel == ADC2_PA0_Channel0)
	{
		/* 使能 ADC1 and GPIOC clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2 | RCC_APB2Periph_GPIOA, ENABLE);

		/* 配置PA0为模拟输入(ADC Channel0) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* 配置ADC1, 不用DMA, 用软件触发 */
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = ENABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		
		ADC_InitStructure.ADC_NbrOfChannel = 1;
		
		ADC_Init(ADC2, &ADC_InitStructure);

		/* 配置ADC1 规则通道2 channel2 configuration */
		ADC_RegularChannelConfig(ADC2, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	}
	
	/* 使能ADC1 DMA功能 */
	ADC_DMACmd(ADC2, ENABLE);

	/* 使能 ADC1 */
	ADC_Cmd(ADC2, ENABLE);

	/* 使能ADC1 复位校准寄存器 */
	ADC_ResetCalibration(ADC2);
	/* 检查ADC1的复位寄存器 */
	while(ADC_GetResetCalibrationStatus(ADC2));

	/* 启动ADC1校准 */
	ADC_StartCalibration(ADC2);
	/* 检查校准是否结束 */
	while(ADC_GetCalibrationStatus(ADC2));

	/* 软件启动ADC转换 */
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);
}





/*
*********************************************************************************************************
*	函 数 名: GetADC
*	功能说明: 读取ADC采样的平均值
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void AdcPro(void)
{
	static u16 buf[SAMP_COUNT];
	static u8 write;
	u32 sum;
	u8 i;
	
	
	{
		buf[write] = ADC_GetConversionValue(ADC1);
	}
	
	
	if (++write >= SAMP_COUNT)
	{
		write = 0;
	}

	/* 
		下面这段代码采用求平均值的方法进行滤波
		也可以改善下，选择去掉最大和最下2个值，使数据更加精确
	*/
	sum = 0;
	for (i = 0; i < SAMP_COUNT; i++)
	{
		sum += buf[i];
	}
	g_usAdcValue = sum / SAMP_COUNT;	/* ADC采样值由若干次采样值平均 */

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	/* 软件启动下次ADC转换 */
}

/*
*********************************************************************************************************
*	函 数 名: GetADC
*	功能说明: 读取ADC采样的平均值
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
 u16 GetADC(void)
{
	u16 ret;
//	OS_CPU_SR  cpu_sr = 0;

	/* 因为	g_AdcValue 变量在systick中断中改写，
		 为了避免主程序读变量时被中断程序打乱导致数据错误，因此需要
		 关闭中断进行保护 
	*/

	/* 进行临界区保护，关闭中断 */
	//__set_PRIMASK(1);  /* 关中断 */
//	OS_ENTER_CRITICAL();

	ret = g_usAdcValue;
	
//	OS_EXIT_CRITICAL();

	//__set_PRIMASK(0);  /* 开中断 */

	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: ADC2Pro
*	功能说明: 读取ADC采样的平均值
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void Adc2Pro(void)
{
	static u16 buf[SAMP_COUNT];
	static u8 write;
	u32 sum;
	u8 i;
	

	{
		buf[write] = ADC_GetConversionValue(ADC2);
	}
	
	if (++write >= SAMP_COUNT)
	{
		write = 0;
	}

	/* 
		下面这段代码采用求平均值的方法进行滤波
		也可以改善下，选择去掉最大和最下2个值，使数据更加精确
	*/
	sum = 0;
	for (i = 0; i < SAMP_COUNT; i++)
	{
		sum += buf[i];
	}
	g_usAdc2Value = sum / SAMP_COUNT;	/* ADC采样值由若干次采样值平均 */

	ADC_SoftwareStartConvCmd(ADC2, ENABLE);	/* 软件启动下次ADC转换 */
}

/*
*********************************************************************************************************
*	函 数 名: GetADC
*	功能说明: 读取ADC采样的平均值
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
 u16 GetADC2(void)
{
	u16 ret;
//	OS_CPU_SR  cpu_sr = 0;


	/* 进行临界区保护，关闭中断 */
	//__set_PRIMASK(1);  /* 关中断 */
	//OS_ENTER_CRITICAL();

	ret = g_usAdc2Value;
	
	//OS_EXIT_CRITICAL();

	//__set_PRIMASK(0);  /* 开中断 */

	return ret;
}


