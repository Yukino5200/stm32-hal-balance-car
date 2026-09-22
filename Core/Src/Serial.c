#include "Serial.h"

#include <stdio.h>
#include <stdarg.h>

#include "main.h"
#include "usart.h"

/*
 * 该函数用于串口发送一个字节
 * 参数：Byte 要发送的字节
 * 返回值：无
 */
void Serial_SendByte(uint8_t Byte)
{
    HAL_UART_Transmit(&huart1, &Byte,1,100 );  // 将数据写入发送数据寄存器
}

/*
 * 该函数用于串口发送一个字节数组
 * 参数：Array 要发送的字节数组
 *      Length 要发送的字节数组长度
 * 返回值：无
 */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i ++)
    {
        Serial_SendByte(Array[i]);  // 逐个发送数组中的字节
    }
}

/*
 *该函数用于串口发送一个字符串
 * 参数：String 要发送的字符串
 * 返回值：无
 */
void Serial_SendString(char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i ++)  // 遍历字符串直到结束符
    {
        Serial_SendByte(String[i]);  // 发送每个字符
    }
}

/*
 * 该函数用于计算X的Y次幂
 * 参数：X 底数
 *      Y 指数
 * 返回值：X的Y次幂
 */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y --)  // 循环Y次
    {
        Result *= X;  // 累乘计算幂
    }
    return Result;
}

/*
 * 该函数用于串口发送一个数字
 * 参数：Number 要发送的数字
 *      Length 要发送的数字的位数
 * 返回值：无
 */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i ++)
    {
        // 分解数字的每一位：先除以对应位的权值，再取模10得到该位数字，最后转换为ASCII码
        Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/*
 * 该函数用于重定向printf函数的输出到串口
 * 参数：ch 要发送的字符
 *      f 文件指针（无用）
 * 返回值：发送的字符
 */
int fputc(int ch, FILE *f)
{
    Serial_SendByte(ch);  // 发送字符到串口
    return ch;  // 返回发送的字符
}

void Serial_Printf(char *format, ...)
{
    char String[100];  // 临时缓冲区
    va_list arg;       // 可变参数列表
    va_start(arg, format);  // 初始化可变参数
    vsprintf(String, format, arg);  // 格式化字符串到缓冲区
    va_end(arg);       // 结束可变参数
    Serial_SendString(String);  // 发送格式化后的字符串
}

/*
 *该函数用于串口1接收中断服务函数
 * 参数：huart 串口1的句柄
 */