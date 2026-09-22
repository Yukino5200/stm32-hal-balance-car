//
// Created by lenovo on 2026/9/16.
//

#ifndef HAL_SERIAL_H
#define HAL_SERIAL_H

#include "main.h"

/* 串口接收数据缓冲区，用于存储接收到的数据包 */
extern char Serial_RxPacket[];

/* 串口接收标志位，1表示接收到完整数据包，读取后需手动清零 */
extern uint8_t Serial_RxFlag;

/* 发送单个字节数据，等待发送完成 */
void Serial_SendByte(uint8_t Byte);

/* 发送字节数组，参数：Array-数组指针，Length-数组长度 */
void Serial_SendArray(uint8_t *Array, uint16_t Length);

/* 发送字符串，以'\0'为结束标志 */
void Serial_SendString(char *String);

/* 发送指定长度的数字，不足位补0，参数：Number-数字，Length-显示位数 */
void Serial_SendNumber(uint32_t Number, uint8_t Length);

/* 格式化输出函数，类似printf，支持可变参数 */
void Serial_Printf(char *format, ...);

#endif //HAL_SERIAL_H
