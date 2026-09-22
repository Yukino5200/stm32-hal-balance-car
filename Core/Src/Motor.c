#include "../Inc/PWM.h"
#include "../Inc/main.h"

/*
 * 该函数用于设置电机的PWM
 * n:电机编号，1为左电机，2为右电机
 * PWM:电机的PWM值，范围为-99~99，正数为正转，负数为反转
 */
void Motor_SetPWM(uint8_t n, int8_t PWM)
{
    if (n == 1)			//指定左电机
    {
        if (PWM >= 0)							//如果设置正转的PWM
        {
            HAL_GPIO_WritePin(GPIOB, AIN1_Pin, GPIO_PIN_SET);	//PB12置高电平
            HAL_GPIO_WritePin(GPIOB, AIN2_Pin, GPIO_PIN_RESET);	//PB13置低电平
            PWM_SetCompare1(PWM);				//设置PWM占空比
        }
        else									//否则，即设置反转的PWM
        {
            HAL_GPIO_WritePin(GPIOB, AIN1_Pin, GPIO_PIN_RESET);	//PB12置低电平
            HAL_GPIO_WritePin(GPIOB, AIN2_Pin, GPIO_PIN_SET);	//PB13置高电平
            PWM_SetCompare1(-PWM);				//设置PWM占空比
        }
    }
    else if (n == 2)	//指定右电机
    {
        if (PWM >= 0)							//如果设置正转的PWM
        {
            HAL_GPIO_WritePin(GPIOB, BIN1_Pin, GPIO_PIN_RESET);	//PB14置低电平
            HAL_GPIO_WritePin(GPIOB, BIN2_Pin, GPIO_PIN_SET);	//PB15置高电平
            PWM_SetCompare2(PWM);				//设置PWM占空比
        }
        else									//否则，即设置反转的PWM
        {
            HAL_GPIO_WritePin(GPIOB, BIN1_Pin, GPIO_PIN_SET);	//PB14置高电平
            HAL_GPIO_WritePin(GPIOB, BIN2_Pin, GPIO_PIN_RESET);	//PB15置低电平
            PWM_SetCompare2(-PWM);				//设置PWM占空比
        }
    }
}

