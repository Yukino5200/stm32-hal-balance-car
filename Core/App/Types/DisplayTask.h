//
// OLED 显示任务模块
//

#ifndef APP_DISPLAYTASK_H
#define APP_DISPLAYTASK_H

#include "cmsis_os.h"

/* 建显示任务：每 50ms 刷一次 OLED，并兼任运行状态指示灯 */
void DisplayTaskCreate(void);

#endif /* APP_DISPLAYTASK_H */
