//
// 装配层：只负责把三个任务建起来，各自的具体实现在同目录的对应文件里。
//

#include "app_tasks.h"
#include "AppState.h"
#include "CommTask.h"
#include "DisplayTask.h"
#include "KeyTask.h"

void App_TasksCreate(void)
{
  AppState_Init();      /* 蓝牙暂存区 = 实时 PID 初值。此时调度器还没启动，不存在并发 */
  CommTaskCreate();     /* 蓝牙队列在 CommTaskCreate() 里建；建完之前 UART 中断投队列会被 NULL 判断挡掉 */
  KeyTaskCreate();
  DisplayTaskCreate();
}
