//
// 蓝牙通信任务模块
//
// 队列的“所有权”归本模块：定义在 CommTask.c，声明在这里。
// usart.c 的 UART2 接收中断是生产者，所以它也要 include 本头文件。
//

#ifndef APP_COMMTASK_H
#define APP_COMMTASK_H

#include "cmsis_os.h"

/* 蓝牙数据包：UART 中断收到一包后投进队列，CommTask 取出来解析 */
#define BLUE_PACKET_LEN   100

typedef struct {
  char buf[BLUE_PACKET_LEN];
} BluePacket_t;

/* 蓝牙接收队列。生产者：usart.c 的 UART2 接收中断；消费者：CommTask。
 * 注意中断可能早于队列创建（UART2_Receive_Start 在 MX_FREERTOS_Init 之前），
 * 所以中断里投队列前必须判 NULL。*/
extern osMessageQueueId_t BlueRxQueue;

/* 建蓝牙队列 + 建通信任务 */
void CommTaskCreate(void);

#endif /* APP_COMMTASK_H */
