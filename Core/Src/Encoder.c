#include "main.h"
#include "tim.h"


/*
 *该函数用于获取编码器的增量值
 *参数n：1-左电机编码器，2-右电机编码器
 *返回值：编码器增量值
 */
int16_t Encoder_Get(uint8_t n)
{
    /*使用Temp变量作为中继，目的是返回CNT后将其清零*/
    int16_t Temp;
    if (n == 1)				                	//指定左电机
    {
        Temp = __HAL_TIM_GET_COUNTER(&htim3);   // 获取编码器计数器值
        __HAL_TIM_SET_COUNTER(&htim3, 0);       // 清零编码器计数器
        return Temp;			                //返回TIM3（左电机编码器）CNT增量值
    }
    else if (n == 2)		                	//指定右电机
    {
        Temp = __HAL_TIM_GET_COUNTER(&htim4);
        __HAL_TIM_SET_COUNTER(&htim4, 0);
        return Temp;			                //返回TIM4（右电机编码器）CNT增量值
    }
    return 0;		                            //参数n非法，默认返回0
}

