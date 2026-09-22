#include <stdio.h>
#include <stdarg.h>
#include "BlueSerial.h"
#include "main.h"
#include "usart.h"

/**
  * 函    数：蓝牙串口发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
void BlueSerial_SendByte(uint8_t Byte)
{
	HAL_UART_Transmit(&huart2, &Byte, 1, 100);		//将字节数据写入数据寄存器，写入后USART自动生成时序波形
}

/**
  * 函    数：蓝牙串口发送一个数组
  * 参    数：Array 要发送数组的首地址
  * 参    数：Length 要发送数组的长度
  * 返 回 值：无
  */
void BlueSerial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)			//遍历数组
	{
		BlueSerial_SendByte(Array[i]);		//依次调用BlueSerial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：蓝牙串口发送一个字符串
  * 参    数：String 要发送字符串的首地址
  * 返 回 值：无
  */
void BlueSerial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)	//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		BlueSerial_SendByte(String[i]);		//依次调用BlueSerial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：次方函数（内部使用）
  * 返 回 值：返回值等于X的Y次方
  */
uint32_t BlueSerial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//设置结果初值为1
	while (Y --)			//执行Y次
	{
		Result *= X;		//将X累乘到结果
	}
	return Result;
}

/**
  * 函    数：蓝牙串口发送数字
  * 参    数：Number 要发送的数字，范围：0~4294967295
  * 参    数：Length 要发送数字的长度，范围：0~10
  * 返 回 值：无
  */
void BlueSerial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)		//根据数字长度遍历数字的每一位
	{
		BlueSerial_SendByte(Number / BlueSerial_Pow(10, Length - i - 1) % 10 + '0');	//依次调用BlueSerial_SendByte发送每位数字
	}
}

/**
  * 函    数：自己封装的prinf函数
  * 参    数：format 格式化字符串
  * 参    数：... 可变的参数列表
  * 返 回 值：无
  */
void BlueSerial_Printf(char *format, ...)
{
	char String[100];				//定义字符数组
	va_list arg;					//定义可变参数列表数据类型的变量arg
	va_start(arg, format);			//从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);					//结束变量arg
	BlueSerial_SendString(String);	//蓝牙串口发送字符数组（字符串）
}
