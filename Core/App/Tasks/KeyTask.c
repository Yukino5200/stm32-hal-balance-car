//
// 按键任务：每 10ms 取一次键码
//
// 启动时不在本任务里做 PID_Init —— 那会一边改 PID 结构体一边被中断读。
// 改成置 StartReq，由 TIM1 中断在安全的时机自己初始化。
//
// K2 同理：它只置 ApplyReq，把"蓝牙暂存参数 → 实时 PID"的提交交给 TIM1 中断。
// 蓝牙收到的参数先落在暂存区（见 AppState.c），所以误发的数据在按 K2 之前不会生效。
//

#include "KeyTask.h"
#include "AppState.h"
#include "Key.h"

static void KeyTask(void *argument)
{
  (void)argument;

  for (;;)
  {
    uint8_t key = Key_GetNum();         /* 只在这里取键码，Key_GetNum 是读后清零的 */

    if (key == 1)                       /* K1：启动 / 停止 */
    {
      if (RunFlag == 0)
      {
        StartReq = 1;                   /* 请求启动，真正的初始化交给 TIM1 中断 */
      }
      else
      {
        RunFlag = 0;                    /* 停止直接置 0 即可 */
      }
    }
    else if (key == 2)                  /* K2：把蓝牙暂存的参数提交给 PID */
    {
      ApplyReq = 1;                     /* 每按一次提交一次，理由同 StartReq */
    }

    osDelay(10);
  }
}

void KeyTaskCreate(void)
{
  static const osThreadAttr_t attr = {
    .name       = "keyTask",
    .stack_size = 96 * 4,               /* CMSIS-RTOS2 的 stack_size 单位是字节，96 字 = 384 字节 */
    .priority   = osPriorityAboveNormal,
  };

  osThreadNew(KeyTask, NULL, &attr);
}
