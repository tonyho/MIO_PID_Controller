#include "Timer3.h"
/*******************************************************************************
* Function Name  : None
* Description    : 定时器3 PWM输出占空比全局变量 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned int T3CH1_DC = 50 ;     // 定时器3通道1占空比 全局变量
unsigned int T3CH2_DC = 50 ;     // 定时器3通道2占空比 全局变量
unsigned int T3CH3_DC = 50 ;     // 定时器3通道3占空比 全局变量
unsigned int T3CH4_DC = 50 ;     // 定时器3通道4占空比 全局变量

/*******************************************************************************
* Function Name  : Timer3_GPIO_Configuration
* Description    : 设置定时器3 PWM输出引脚 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Timer3_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure_TIMER3PWM_PORTA;
    GPIO_InitTypeDef GPIO_InitStructure_TIMER3PWM_PORTB;

    GPIO_InitStructure_TIMER3PWM_PORTA.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
      GPIO_InitStructure_TIMER3PWM_PORTA.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStructure_TIMER3PWM_PORTA.GPIO_Mode = GPIO_Mode_AF_PP;            
      GPIO_Init(GPIOA, &GPIO_InitStructure_TIMER3PWM_PORTA);  

    GPIO_InitStructure_TIMER3PWM_PORTB.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
      GPIO_InitStructure_TIMER3PWM_PORTB.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStructure_TIMER3PWM_PORTB.GPIO_Mode = GPIO_Mode_AF_PP;            
      GPIO_Init(GPIOB, &GPIO_InitStructure_TIMER3PWM_PORTB); 
}

/*******************************************************************************
* Function Name  : Timer3_BASE_Configuration              注意事项：
* Description    : 设置基本参数                              在 TIM_Period 里的5000 为一个重要常量
* Input          : None                                         在下面的 初始化PWM信号 和 调节PWM占空比 函数里都用到
* Output         : None                                         如果要调整PWM输出频率，那上面提到的两个函数里都要调节！
* Return         : None                    
*******************************************************************************/
void Timer3_BASE_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_DeInit(TIM3);                                           //重新将Timer设置为缺省值
    TIM_InternalClockConfig(TIM3);                              //采用内部时钟给TIM3提供时钟源
    TIM_TimeBaseStructure.TIM_Prescaler = 720;                    
    //预分频系数为72，即不进行预分频，此时TIMER的频率为1MHz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;     //设置时钟分割
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //设置计数器模式为向上计数模式
    TIM_TimeBaseStructure.TIM_Period = 5000 - 1;                    
    //设置计数溢出大小，每计5000个数就产生一个更新事件，即PWM的输出频率为50Hz,Fre = 20ms
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);              //将配置应用到TIM3中
} 

/*******************************************************************************
* Function Name  : Timer3_PWN_Configuration
* Description    : 设置PWM参数 
* Input          : Dutyfactor ： 占空比  （0~100）
* Output         : None
* Return         : None
*******************************************************************************/
void Timer3_PWN_Configuration(void)
{
    TIM_OCInitTypeDef TIM_OCInitStructure;

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;               //PWM方式  分1,2

    /*************************** 通道1 ********************************/
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //PWM功能使能
    TIM_OCInitStructure.TIM_Pulse = (T3CH1_DC * 5000) / 100;        //写比较值(占空比输入)
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //设置正反相位，是先高电平还是先低电平
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 

    /*************************** 通道2 ********************************/
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = (T3CH2_DC * 5000) / 100;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

    /*************************** 通道3 ********************************/
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = (T3CH3_DC * 5000) / 100;
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

    /*************************** 通道4 ********************************/
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = (T3CH4_DC * 5000) / 100;
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM3, ENABLE);                       
    TIM_Cmd(TIM3, ENABLE); 
}

/*******************************************************************************
* Function Name  : TIM3PWM_4Channel_Configuration
* Description    : 初始化定时器3，4个通道的PWM输出，占空比初始化为50% 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3PWM_4Channel_Configuration(void)
{
    Timer3_GPIO_Configuration();
    Timer3_BASE_Configuration();
    Timer3_PWN_Configuration();
}

/*******************************************************************************
* Function Name  : Adjust_DutyCycle
* Description    : 定时器3调节4个通道的占空比 
* Input          : CHx : 输出通道   DC : 调节到多少量
* Output         : None
* Return         : None
*******************************************************************************/
void Adjust_TIM3DutyCycle(unsigned char CHx , unsigned int DC)
{
    switch(CHx)
    {
        case CH1 :
        {
            TIM_SetCompare1( TIM3,((DC*5000)/100) );
            break;
        }
        case CH2 :
        {
            TIM_SetCompare2( TIM3,((DC*5000)/100) );
            break;
        }
        case CH3 :
        {
            TIM_SetCompare3( TIM3,((DC*5000)/100) );
            break;
        }
        case CH4 :
        {
            TIM_SetCompare4( TIM3,((DC*5000)/100) );
            break;
        }
        default  :
        {break;}
    }
}