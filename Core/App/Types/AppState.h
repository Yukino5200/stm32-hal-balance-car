//
// 共享应用状态：被 TIM1 中断和 FreeRTOS 任务同时访问的全局变量
//
// 这些变量原先定义在 main.c 的 USER CODE PV 区。搬到这里之后，main.c 里的 PV 区
// 只剩中断私有的临时变量，共享状态有了明确的归属文件。
//
// 为什么只加 volatile 不加锁：Cortex-M3 上对齐的 32 位读写本身是单次原子操作，
// 单个标志位 / 单个 float 的读写不需要临界区。而结构体这种多字段拷贝确实非原子，
// 但混进相邻两个控制周期的值在这些用途上无害（详见各变量的注释）。
//

#ifndef APP_APPSTATE_H
#define APP_APPSTATE_H

#include "main.h"
#include "PID.h"

/* 运行标志：TIM1 中断（倒地保护）和 KeyTask 都会写，DisplayTask 读 */
extern volatile uint8_t RunFlag;
/* 启动请求：KeyTask 置 1，TIM1 中断消费（把 PID_Init 挪进中断，避开并发） */
extern volatile uint8_t StartReq;
/* 应用请求：KeyTask 在 K2 按下时置 1，TIM1 中断把暂存区提交给实时 PID。
 *和 StartReq 同一个套路：任务只发请求，真正改结构体的动作放在没有并发的中断里。 */
extern volatile uint8_t ApplyReq;

extern volatile float Angle;          /* 互补滤波后的角度：ISR 写、DisplayTask 读 */
extern volatile float AveSpeed;       /* 平均速度：ISR 写、DisplayTask 读 */
extern volatile float DifSpeed;       /* 差分速度：ISR 写、DisplayTask 读 */
extern volatile int16_t GY;           /* 陀螺仪 Y 轴原始值（已减零漂）：ISR 写、DisplayTask 读 */

/* 三个 PID 环：ISR 读 Actual 写 Out，CommTask 写 Kp/Ki/Kd/Offset，DisplayTask 只读 */
extern PID_t AnglePID;
extern PID_t SpeedPID;
extern PID_t TurnPID;

/* 蓝牙参数暂存区：CommTask 写、TIM1 中断在 K2 提交时读。声明方式与实时 PID 一致（不加
 *volatile），理由也相同 —— 每个字段都是对齐的单字，提交又发生在中断里，读不到半更新的值。 */
extern PID_t AnglePID_Stage;
extern PID_t SpeedPID_Stage;
extern PID_t TurnPID_Stage;

/* 上电时把暂存区同步成实时 PID 的初值，避免同一批数字写两份 */
void AppState_Init(void);

#endif /* APP_APPSTATE_H */
