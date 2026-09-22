//
// Created by lenovo on 2026/9/14.
//

#ifndef HAL_KEY_H
#define HAL_KEY_H

#include <main.h>

uint8_t Key_GetState(void);
void Key_Tick(void);
uint8_t Key_GetNum(void);

#endif //HAL_KEY_H
