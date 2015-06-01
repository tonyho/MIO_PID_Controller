#include "includes.h"

/*====================================================================================================
PID Function
The PID (比例、积分、微分) function is used in mainly
control applications. PIDCalc performs one iteration of the PID
algorithm.
While the PID function works, main is just a dummy program showing
a typical usage.
=====================================================================================================*/

/*====================================================================================================/
PID计算部分
typedef struct PID {
	float SetPoint;    // 设定目标Desired value
	float Proportion; // 比例常数Proportional Const
	float Integral;   // 积分常数Integral Const
	float Derivative; // 微分常数Derivative Const
	float LastError;  // Error[-1]	
	float PrevError;  // Error[-2]
	float SumError;   // Sums of Errors
}PID;
=====================================================================================================*/

PID   value_PID[6];
extern float E_Percent[2];
/*
*********************************************************************************************************
*                                    PID Calc RESPONSE TASK
*
* Description : This task monitors the AD and Get the AD Value
*
* Arguments   : M=[0 1 2 3 4 5 ]  
*
* Returns     : none
*********************************************************************************************************
*/
u16 PIDCalc(u16 value_set,u16 value_measurement,unsigned int M)
{
	u16 value_out; // PID Response (Output)
	float NextPoint; // PID Feedback (Input)
	float dError,Error;
	
	value_PID[M].SetPoint = (float)value_set; // 设定值
	NextPoint = (float)value_measurement; // 测量值
	Error = value_PID[M].SetPoint - NextPoint; // 偏差

	if(0 == M || 2 == M || 4 == M)
	{
		Error = Error * E_Percent[0];		
	}
	else 
	{
		Error = Error * E_Percent[1];
	}
	
	value_PID[M].SumError += Error; // 积分
	dError = value_PID[M].LastError - value_PID[M].PrevError; // 当前微分
	value_PID[M].PrevError = value_PID[M].LastError;
	value_PID[M].LastError = Error;
	
	//!< 限幅
  {
		if(value_PID[M].SumError>150)  
		{
			value_PID[M].SumError=150;
		}
		if(value_PID[M].SumError<-150)	  
		{  
			value_PID[M].SumError=-150;
		}
	}
	
	//!< 完成PID计算
	{
	value_out = (u16)(value_PID[M].Proportion * Error + value_PID[M].Integral * value_PID[M].SumError 
							 + value_PID[M].Derivative * dError);
	}
	
	//!< 限幅  因为PWM最大为255 最小即为0
	{
		if(value_out>=255)
		{
			value_out=255;
		}
		else
		{		
			 if(value_out<=0)
			 {
				 value_out=0; // 可否改成0? 
			 }
		}
	}

	return value_out;
}



