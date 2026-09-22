//
// Created by lenovo on 2026/9/16.
//

#ifndef HAL_BLUESERIAL_H
#define HAL_BLUESERIAL_H

#include <main.h>

void BlueSerial_SendByte(uint8_t Byte);
void BlueSerial_SendArray(uint8_t *Array, uint16_t Length);
void BlueSerial_SendString(char *String);
void BlueSerial_SendNumber(uint32_t Number, uint8_t Length);
void BlueSerial_Printf(char *format, ...);

#endif //HAL_BLUESERIAL_H
