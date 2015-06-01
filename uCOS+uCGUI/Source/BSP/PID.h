#ifndef __PID_H
#define __PID_H
typedef struct PID {
	float SetPoint;    // 设定目标Desired value
	float Proportion; // 比例常数Proportional Const
	float Integral;   // 积分常数Integral Const
	float Derivative; // 微分常数Derivative Const
	float LastError;  // Error[-1]	
	float PrevError;  // Error[-2]
	float SumError;   // Sums of Errors
}PID;
		  
u16 PIDCalc(u16 value_set,u16 value_measurement,unsigned int M);


#endif
