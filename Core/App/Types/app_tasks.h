//
// 应用任务层的装配入口
//
// 本目录（Core/App/）下的文件都不是 CubeMX 生成的，重新生成代码不会被覆盖：
//   AppState.h/.c    被中断和任务共享的全局变量
//   KeyTask.h/.c     按键任务
//   CommTask.h/.c    蓝牙通信任务（含队列的定义）
//   DisplayTask.h/.c OLED 显示任务
//
// PID 控制环不在这里 —— 它留在 TIM1 中断里（10ms 角度环、50ms 速度环和转向环），
// 硬实时的东西不该受调度器影响。
//

#ifndef APP_TASKS_H
#define APP_TASKS_H

/* 建蓝牙队列 + 建按键 / 通信 / 显示三个任务。
 * 在 freertos.c 的 MX_FREERTOS_Init() 的 USER CODE 区调用。*/
void App_TasksCreate(void);

#endif /* APP_TASKS_H */
