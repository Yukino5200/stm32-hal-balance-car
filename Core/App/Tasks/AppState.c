//
// 共享应用状态的定义。声明（extern）在 AppState.h。
//

#include "AppState.h"

volatile uint8_t RunFlag;			//运行标志位：TIM1中断（倒地保护）和KeyTask都会写
volatile uint8_t ApplyReq;			//应用请求：KeyTask在K2按下时置1，TIM1中断把暂存区提交给实时PID
volatile uint8_t StartReq;			//启动请求：KeyTask置1，TIM1中断消费

volatile float Angle;				//互补滤波后的角度值，准确且无漂移
volatile float AveSpeed, DifSpeed;	//平均速度，差分速度
volatile int16_t GY;				//陀螺仪Y轴原始值，已减去零漂

/*定义PID结构体变量，定义的时候同时给部分成员赋初值*/
PID_t AnglePID = {					//角度环PID结构体变量
  .Kp = 5,							//比例项权重
  .Ki = 0.1,						//积分项权重
  .Kd = 5,							//微分项权重

  .OutMax = 100,					//输出限幅的最大值
  .OutMin = -100,					//输出限幅的最小值

  .OutOffset = 3,					//输出偏移

  .ErrorIntMax = 600,				//误差积分的最大值
  .ErrorIntMin = -600,				//误差积分的最小值
};

PID_t SpeedPID = {					//速度环PID结构体变量
  .Kp = 2,							//比例项权重
  .Ki = 0.05,						//积分项权重
  .Kd = 0,							//微分项权重

  .OutMax = 20,						//输出限幅的最大值
  .OutMin = -20,					//输出限幅的最小值

  .ErrorIntMax = 150,				//误差积分的最大值
  .ErrorIntMin = -150,				//误差积分的最小值
};

PID_t TurnPID = {					//转向环PID结构体变量
  .Kp = 4,							//比例项权重
  .Ki = 3,							//积分项权重
  .Kd = 0,							//微分项权重

  .OutMax = 50,						//输出限幅的最大值
  .OutMin = -50,					//输出限幅的最小值

  .ErrorIntMax = 20,				//误差积分的最大值
  .ErrorIntMin = -20,				//误差积分的最小值
};

/*蓝牙参数暂存区：CommTask 只写这里，按下 K2 后才由 TIM1 中断整体提交给实时 PID。
 *初值在上电时由 AppState_Init() 从实时 PID 拷过来，避免同一批数字在两处维护。*/
PID_t AnglePID_Stage;
PID_t SpeedPID_Stage;
PID_t TurnPID_Stage;

void AppState_Init(void)
{
	AnglePID_Stage = AnglePID;
	SpeedPID_Stage = SpeedPID;
	TurnPID_Stage  = TurnPID;
}
