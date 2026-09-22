//
// 按键任务模块
//

#ifndef APP_KEYTASK_H
#define APP_KEYTASK_H

#include "cmsis_os.h"

/* 建按键任务：每 10ms 读一次键码，K1 触发启动 / 停止 */
void KeyTaskCreate(void);

#endif /* APP_KEYTASK_H */
