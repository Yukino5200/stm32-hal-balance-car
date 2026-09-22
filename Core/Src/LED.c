#include "stm32f103xb.h"
#include "GPIO.h"

// 该端口LED灯的长脚接VCC，短脚接PC13引脚，LED灯亮时PC13输出低电平，灭时输出高电平
void LED_ON(void)
{
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);			//设置PC13引脚为低电平
}

void LED_OFF(void)
{
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);			//设置PC13引脚为高电平
}

void LED_Turn(void)
{
    if (HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin) == 0)	//获取输出寄存器的状态，如果当前引脚输出低电平
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);					//设置PC13引脚为高电平
    }
    else													//否则，即当前引脚输出高电平
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);					//设置PC13引脚为低电平
    }
}