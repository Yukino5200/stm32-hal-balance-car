#include <stdint.h>

#include "main.h"
//
// Created by lenovo on 2026/9/14.
//
uint8_t Key_Num;		//定义一个全局变量，用于存储键码

/*
 用于获取4个按键的状态（没有消抖）
 */
uint8_t Key_GetState(void)
{
    if (HAL_GPIO_ReadPin(Key1_GPIO_Port, Key1_Pin) == 0)		//如果PB1引脚电平为0
    {
        return 1;		//直接返回键码1
    }
    if (HAL_GPIO_ReadPin(Key2_GPIO_Port, Key2_Pin) == 0)		//如果PB0引脚电平为0
    {
        return 2;		//直接返回键码2
    }
    if (HAL_GPIO_ReadPin(Key3_GPIO_Port, Key3_Pin) == 0)		//如果PA5引脚电平为0
    {
        return 3;		//直接返回键码3
    }
    if (HAL_GPIO_ReadPin(Key4_GPIO_Port, Key4_Pin) == 0)		//如果PA4引脚电平为0
    {
        return 4;		//直接返回键码4
    }
    return 0;			//没有if成立，表示没有按键按下，默认返回0
}

/*
    该函数需要放在TIM1的中断服务函数中，每隔1ms调用一次,也就是下面的if语句每隔20ms进一次，定时器可以代替delay函数进行非阻塞式消抖（delay会阻塞cpu运行程序）
    该函数检测到按键松手瞬间，就将按键的键码保存到全局变量Key_Num中，后续函数读取此变量，即可得知哪个按键按下了（同样也防止了按住按键不松导致的重复启用）
 */
void Key_Tick(void)
{
    /*定义静态变量（默认初值为0，函数退出后保留值和存储空间）*/
    static uint8_t Count;					//用于计次分频
    static uint8_t CurrState, PrevState;	//保存按键本次状态和上次状态

    Count ++;			//计次自增
    if (Count >= 20)	//如果计次20次，则if成立，即if每隔20ms进一次
    {
        Count = 0;		//计次清零，便于下次计次

        /*获取按键的本次状态和上次状态*/
        PrevState = CurrState;			//获取上次状态
        CurrState = Key_GetState();		//获取本次状态

        /*如果本次状态的键码为0，且上次键码不为0，即检测到按键松手瞬间*/
        if (CurrState == 0 && PrevState != 0)
        {
            /*将上次状态的键码复制给全局变量，后续读取此变量，即可得知哪个按键按下了*/
            Key_Num = PrevState;
        }
    }
}

/*
    该函数用于读取按键的键码，并在读取后将键码清零，防止重复读取（主要是为了读取键码后清零）
 */

uint8_t Key_GetNum(void)
{
    uint8_t Temp;			//定义一个临时变量用于中转
    if (Key_Num)			//如果全局变量的键码不为0
    {
        /*这3句的目的是，实现读取键码并读后清零的效果*/
        Temp = Key_Num;		//先把键码存入临时变量
        Key_Num = 0;		//键码清零
        return Temp;		//返回临时变量，return语句执行后，函数直接结束
    }
    return 0;				//如果if不成立，键码为0，则默认返回0
}
