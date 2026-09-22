//
// 通信任务：阻塞等蓝牙队列，解析数据包
// 数据包格式：[标签,数据1,数据2,...]
//
// 解析出来的 PID 参数只写暂存区（AnglePID_Stage 等），不直接改实时 PID。
// 按下 K2 后由 TIM1 中断把暂存区整体提交过去 —— 这样误发的滑杆值不会立刻生效。
//

#include "CommTask.h"
#include "AppState.h"

#include <string.h>
#include <stdlib.h>

osMessageQueueId_t BlueRxQueue;

static void CommTask(void *argument)
{
  BluePacket_t packet;

  (void)argument;

  for (;;)
  {
    if (osMessageQueueGet(BlueRxQueue, &packet, NULL, osWaitForever) != osOK)
    {
      continue;
    }

    char *tag = strtok(packet.buf, ",");          /* 提取标签 */
    if (tag == NULL)
    {
      continue;
    }

    if (strcmp(tag, "key") == 0)                  /* 收到按键数据包 */
    {
      /*此处可执行按键操作，目前程序暂时没用到按键*/
    }
    else if (strcmp(tag, "slider") == 0)          /* 收到滑杆数据包 */
    {
      char *name  = strtok(NULL, ",");            /* 滑杆名称 */
      char *value = strtok(NULL, ",");            /* 滑杆值 */

      if (name == NULL || value == NULL)
      {
        continue;
      }

      /*写进暂存区，实时 PID 一个字段都不碰*/
      float v = atof(value);
      if (strcmp(name, "AngleKp") == 0)           AnglePID_Stage.Kp = v;
      else if (strcmp(name, "AngleKi") == 0)      AnglePID_Stage.Ki = v;
      else if (strcmp(name, "AngleKd") == 0)      AnglePID_Stage.Kd = v;
      else if (strcmp(name, "SpeedKp") == 0)      SpeedPID_Stage.Kp = v;
      else if (strcmp(name, "SpeedKi") == 0)      SpeedPID_Stage.Ki = v;
      else if (strcmp(name, "SpeedKd") == 0)      SpeedPID_Stage.Kd = v;
      else if (strcmp(name, "TurnKp")  == 0)      TurnPID_Stage.Kp  = v;
      else if (strcmp(name, "TurnKi")  == 0)      TurnPID_Stage.Ki  = v;
      else if (strcmp(name, "TurnKd")  == 0)      TurnPID_Stage.Kd  = v;
      else if (strcmp(name, "Offset")  == 0)      AnglePID_Stage.OutOffset = v;
    }
    else if (strcmp(tag, "joystick") == 0)        /* 收到摇杆数据包 */
    {
      strtok(NULL, ",");                          /* 数据2 LH，未使用 */
      char *lv = strtok(NULL, ",");               /* 数据3 LV */
      char *rh = strtok(NULL, ",");               /* 数据4 RH */
      strtok(NULL, ",");                          /* 数据5 RV，未使用 */

      if (lv == NULL || rh == NULL)
      {
        continue;
      }

    SpeedPID.Target = (int8_t)atoi(lv) / 25.0;  /* 前后行进控制 */
    TurnPID.Target  = (int8_t)atoi(rh) / 25.0;  /* 左右转弯控制 */
    }
  }
}

void CommTaskCreate(void)
{
  /* 先建队列再建任务，保证 CommTask 一开跑就有队列可取 */
  BlueRxQueue = osMessageQueueNew(4, sizeof(BluePacket_t), NULL);

  static const osThreadAttr_t attr = {
    .name       = "commTask",
    .stack_size = 512 * 4,              /* 2048 字节：strtok + atof(软浮点) + strcmp 链 */
    .priority   = osPriorityNormal,
  };

  osThreadNew(CommTask, NULL, &attr);
}
