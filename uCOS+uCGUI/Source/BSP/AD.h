#ifndef __AD__
#define __AD__

#define __IO volatile
#define SAMP_COUNT	20		/* 样本个数，表示200ms内的采样数据求平均值 */


#define ADC1_PC4_Channel1 1
#define ADC1_PA2_Channel2 2
#define ADC1_PA0_Channel0 3

#define ADC2_PC4_Channel1 1
#define ADC2_PA2_Channel2 2
#define ADC2_PA0_Channel0 3



void ADC_Configuration(unsigned int Channel);
void AdcPro(void);
u16 GetADC(void);

void ADC2_Configuration(unsigned int Channel);
void Adc2Pro(void);
u16 GetADC2(void);

#endif
