//
// 显示任务：每 50ms 刷新一次 OLED，并兼任运行状态指示灯
//
// 渲染前先把 ISR 正在改的变量拷到任务本地副本，之后只用副本。
// 每个字段都是对齐的 32 位读，本身就是原子的，不需要临界区；
// 而且绝对不能加临界区 —— TIM1 优先级是 5，会被一起屏蔽掉。
//

#include "DisplayTask.h"
#include "AppState.h"
#include "LED.h"
#include "OLED.h"

static void DisplayTask(void *argument)
{
  (void)argument;

  for (;;)
  {
    /* 快照 */
    PID_t ang = AnglePID;
    PID_t spd = SpeedPID;
    PID_t trn = TurnPID;
    float angle  = Angle;
    float aveSpd = AveSpeed;
    float difSpd = DifSpeed;
    int16_t gy   = GY;

    /*LED指示程序运行状态*/
    if (RunFlag)
    {
      LED_ON();
    }
    else
    {
      LED_OFF();
    }

    /*OLED显示*/
    OLED_Clear();         // 有的值会变化会超出预设的空间，超出的部分只能靠清零
    OLED_Printf(0, 0, OLED_6X8, "  Angle");						//OLED左侧显示角度环参数
    OLED_Printf(0, 8, OLED_6X8, "P:%05.2f", ang.Kp);		//角度环Kp
    OLED_Printf(0, 16, OLED_6X8, "I:%05.2f", ang.Ki);		//角度环Ki
    OLED_Printf(0, 24, OLED_6X8, "D:%05.2f", ang.Kd);		//角度环Kd
    OLED_Printf(0, 32, OLED_6X8, "T:%+05.1f", ang.Target);	//角度环目标值
    OLED_Printf(0, 40, OLED_6X8, "A:%+05.1f", angle);			//角度环实际值
    OLED_Printf(0, 48, OLED_6X8, "O:%+05.0f", ang.Out);	//角度环输出值
    OLED_Printf(0, 56, OLED_6X8, "GY:%+05d", gy);				//显示GY，便于校准陀螺仪零漂
    OLED_Printf(56, 56, OLED_6X8, "Offset:%02.0f", ang.OutOffset);		//显示输出偏移值
    OLED_Printf(50, 0, OLED_6X8, "Speed");						//OLED中间显示速度环参数
    OLED_Printf(50, 8, OLED_6X8, "%05.2f", spd.Kp);			//速度环Kp
    OLED_Printf(50, 16, OLED_6X8, "%05.2f", spd.Ki);		//速度环Ki
    OLED_Printf(50, 24, OLED_6X8, "%05.2f", spd.Kd);		//速度环Kd
    OLED_Printf(50, 32, OLED_6X8, "%+05.1f", spd.Target);	//速度环目标值
    OLED_Printf(50, 40, OLED_6X8, "%+05.1f", aveSpd);			//速度环实际值
    OLED_Printf(50, 48, OLED_6X8, "%+05.0f", spd.Out);		//速度环输出值
    OLED_Printf(88, 0, OLED_6X8, "Turn");						//OLED右侧显示转向环参数
    OLED_Printf(88, 8, OLED_6X8, "%05.2f", trn.Kp);			//转向环Kp
    OLED_Printf(88, 16, OLED_6X8, "%05.2f", trn.Ki);			//转向环Ki
    OLED_Printf(88, 24, OLED_6X8, "%05.2f", trn.Kd);			//转向环Kd
    OLED_Printf(88, 32, OLED_6X8, "%+05.1f", trn.Target);	//转向环目标值
    OLED_Printf(88, 40, OLED_6X8, "%+05.1f", difSpd);			//转向环实际值
    OLED_Printf(88, 48, OLED_6X8, "%+05.0f", trn.Out);		//转向环输出值
    OLED_Update();				//OLED更新

    osDelay(50);
  }
}

void DisplayTaskCreate(void)
{
  static const osThreadAttr_t attr = {
    .name       = "displayTask",
    .stack_size = 640 * 4,              /* 2560 字节：22 次 OLED_Printf，每次带 256 字节缓冲 + 软浮点 vsprintf */
    .priority   = osPriorityBelowNormal,
  };

  osThreadNew(DisplayTask, NULL, &attr);
}
