#include <stdint.h>

#include "tim.h"

/*
 * 该函数用于设置PA0的PWM的占空比
 * Compare:占空比的值，范围为0~99，对应0%~100%
 */
void PWM_SetCompare1(uint16_t Compare)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, Compare);		//设置CCR1的值
}

/*
 * 该函数用于设置PA1的PWM的占空比
 * Compare:占空比的值，范围为0~99，对应0%~100%
 */
void PWM_SetCompare2(uint16_t Compare)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, Compare);        //设置CCR2的值
}
