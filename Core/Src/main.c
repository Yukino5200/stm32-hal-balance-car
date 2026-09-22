/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "LED.h"
#include "Key.h"
#include "MPU6050.h"
#include "Serial.h"
#include "BlueSerial.h"
#include "Encoder.h"
#include "Motor.h"
#include "PID.h"
#include "AppState.h"      /* RunFlag / StartReq / Angle / AveSpeed / DifSpeed / GY / 三个 PID 结构体 */

#include <math.h>
#include <string.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* 中断私有的临时变量：只有本文件的 TIM1 中断用，任务不碰，所以留在本地。
 * 被"中断 + 任务"共享的变量（RunFlag / StartReq / Angle / AveSpeed / DifSpeed / GY
 * 和三个 PID 结构体）已经搬到 Core/App/AppState.c 里去了。*/
int16_t AX, AY, AZ, GX, GZ;			//读取MPU6050的原始数据
uint16_t TimerCount;				//定时器计数值，此值可用于计算定时中断函数具体的执行时间

float AngleAcc;						//由加速度计得到的角度值
float AngleGyro;					//由陀螺仪得到的角度值，执行互补滤波后，此值基本与Angle相等

int16_t LeftPWM, RightPWM;			//左PWM，右PWM
int16_t AvePWM, DifPWM;				//平均PWM，差分PWM

float LeftSpeed, RightSpeed;		//左速度，右速度

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();
  HAL_TIM_Base_Start_IT(&htim1);							// 启动TIM1并使能更新中断
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);          // 启动左电机PWM
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);          // 启动右电机PWM
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);    // 启动左编码器
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);    // 启动右编码器
  UART1_Receive_Start();									// 启动UART1并使能接收中断
  UART2_Receive_Start();									 // 启动UART2并使能接收中断
  MPU6050_Init();											// MPU6050初始化



  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /* 应用逻辑（LED/按键/OLED/蓝牙解析）已搬进 FreeRTOS 任务，见 app_tasks.c；
   * PID 控制环仍留在 TIM1 中断里，硬实时的东西不受调度器影响 */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/*
 *回调函数：里面写入中断处理的代码，不需要清理标志位，Hal库会自动清理
 *htim：定时器句柄
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1)   // 选择TIM1
  {
    static uint16_t Count0, Count1;
    int16_t GYraw;			//MPU6050原始值的临时接收变量。GY是volatile的，不能把它的地址交给库函数（会丢掉volatile限定符）
    Key_Tick();
  	/*计次分频*/
  	Count0 ++;				//计次自增
  	if (Count0 >= 10)		//如果计次10次，则if成立，即if每隔10ms进一次
  	{
  		Count0 = 0;			//计次清零，便于下次计次

  		/*启动请求：keyTask 置 StartReq，真正的 PID_Init 在这里做。
  		 *原因：PID_Init 一次清零 15 个字段，和中断并发会读到半新半旧的参数；
  		 *而任务侧又不能加临界区（TIM1 优先级 5 会被一起屏蔽）。中断里本来就没有并发。*/
  		if (StartReq)
  		{
  			PID_Init(&AnglePID);
  			PID_Init(&SpeedPID);
  			PID_Init(&TurnPID);
  			RunFlag = 1;
  			StartReq = 0;
  		}

  		/*应用请求：keyTask 在 K2 按下时置 ApplyReq，把蓝牙暂存区整体提交给实时 PID。
  		 *和 StartReq 同样的道理 —— 一次改 15 个字段，放在中断里做就没有并发。*/
  		if (ApplyReq)
  		{
  			AnglePID = AnglePID_Stage;
  			SpeedPID = SpeedPID_Stage;
  			TurnPID  = TurnPID_Stage;
  			ApplyReq = 0;
  		}

  		/*在中断里读取MPU6050，可以保证读取间隔严格为1ms*/
  		/*但要保证MPU6050_GetData执行时间不超过1ms*/
  		MPU6050_GetData(&AX, &AY, &AZ, &GX, &GYraw, &GZ);

  		/*校准陀螺仪Y轴零漂*/
  		/*此值需实测确定，不同的设备零漂一般不同*/
  		/*实测方法是，在完全静止时，观察OLED显示的GY值，即为零漂值*/
  		/*然后在此处将零漂值减去，使得完全静止时，GY值为0*/
  		GY = GYraw - 16;	//原始值减零漂后写入volatile变量（一次写完，displayTask不会读到写了一半的值）

  		/*由加速度计计算得到角度值*/
  		/*atan2计算反正切，得到角度（弧度制）， / 3.14159 * 180可将弧度制转为角度值*/
  		AngleAcc = -atan2(AX, AZ) / 3.14159 * 180;

  		/*校准中心角度*/
  		/*此值需实测确定，不同的设备中心角度一般不同*/
  		/*实测方法是，使平衡车绝对竖直，观察OLED显示的角度环实际值，即为中心角度偏移值*/
  		/*然后在此处将偏移值减去，使得绝对竖直时，角度环实际值为0*/
  		AngleAcc += 0.5;

  		/*由陀螺仪积分得到角度值*/
  		/*互补滤波下，角度积分要在上次滤波后的Angle上进行*/
  		/*公式中32768是int16_t变量的最大值，2000是陀螺仪配置的满量程2000度每秒，0.01是间隔时间10ms*/
  		AngleGyro = Angle + GY / 32768.0 * 2000 * 0.01;

  		/*执行互补滤波*/
  		float Alpha = 0.01;		//互补滤波系数，值越大，越偏向于加速度计，值越小，越偏向于陀螺仪
  		Angle = Alpha * AngleAcc + (1 - Alpha) * AngleGyro;		//互补滤波计算，得到稳定的角度值

  		/*平衡车倒地后自动停止PID程序*/
  		if (Angle > 50 || Angle < -50)	//角度超过-50度~50度的范围，认为平衡车倒地了
  		{
  			RunFlag = 0;				//RunFlag置0，停止PID程序
  		}

  		/*执行PID调控程序*/
  		if (RunFlag)					//RunFlag非0时，启动PID程序
  		{
  			/*角度环PID控制*/
  			AnglePID.Actual = Angle;	//角度环实际值为Angle
  			PID_Update(&AnglePID);		//调用封装好的函数，一步完成PID计算和更新
  			AvePWM = -AnglePID.Out;		//角度环的输出值给到电机平均PWM，用于控制前后行进

  			/*控制量转换*/
  			LeftPWM = AvePWM + DifPWM / 2;		//由平均PWM和差分PWM计算得到左轮PWM
  			RightPWM = AvePWM - DifPWM / 2;		//由平均PWM和差分PWM计算得到右轮PWM

  			/*PWM限幅*/
  			/*上式计算后，LeftPWM和RightPWM可能会超出电机允许的PWM范围，此处将PWM值范围限制在-100~100之内*/
  			if (LeftPWM > 100)
  			{
  				LeftPWM = 100;
  			}
  			else if(LeftPWM < -100)
  			{
  				LeftPWM = -100;
  			}
  			if (RightPWM > 100)
  			{
  				RightPWM = 100;
  			}
  			else if (RightPWM < -100)
  			{
  				RightPWM = -100;
  			}

  			/*PWM输出给电机*/
  			Motor_SetPWM(1, LeftPWM);		//LeftPWM输出给左轮电机
  			Motor_SetPWM(2, RightPWM);		//RightPWM输出给右轮电机
  		}
  		else		//RunFlag为0时，停止PID程序
  		{
  			/*左右电机PWM均设置为0*/
  			Motor_SetPWM(1, 0);
  			Motor_SetPWM(2, 0);
  		}
  	}
  	/*计次分频*/
    Count1 ++;
    if (Count1 >= 50)		//如果计次50次，则if成立，即if每隔50ms进一次
    {
      Count1 = 0;			//计次清零，便于下次计次

      /*获取编码器的计次值增量，并计算电机旋转速度*/
      /*Encoder_Get函数，可以获取两次读取编码器的计次值增量*/
      /*编码器磁铁旋转一圈，计次增量为44，编码器读取间隔是50ms（0.05s）*/
      /*因此磁铁旋转速度 = 计次增量 / 44 / 0.05，单位是转每秒*/
      /*平衡车使用的电机带有减速箱，减速比为9.27666*/
      /*因此电机输出轴旋转速度 = 磁铁旋转速度 / 9.27666，单位是转每秒*/
      /*左轮编码器方向与右轮相反(实测两轮同向前进时L为负、R为正)，故取负对齐*/
      LeftSpeed = -Encoder_Get(1) / 44.0 / 0.05 / 9.27666;
      RightSpeed = Encoder_Get(2) / 44.0 / 0.05 / 9.27666;
      /*信号量转换*/
      AveSpeed = (LeftSpeed + RightSpeed) / 2.0;	//由左轮速度和右轮速度计算得到平均速度
      DifSpeed = LeftSpeed - RightSpeed;			//由左轮速度和右轮速度计算得到差分速度
    	// DifSpeed = 0;
      /*执行PID调控程序*/
      if (RunFlag)					//RunFlag非0时，启动PID程序
      {
        /*速度环PID控制*/
        SpeedPID.Actual = AveSpeed;			//速度环实际值为AveSpeed
        PID_Update(&SpeedPID);				//调用封装好的函数，一步完成PID计算和更新
        AnglePID.Target = SpeedPID.Out;		//速度环的输出值给到角度环的目标值，构成串级PID

        /*转向环PID控制*/
        TurnPID.Actual = DifSpeed;			//转向环实际值为DifSpeed
        PID_Update(&TurnPID);				//调用封装好的函数，一步完成PID计算和更新
        DifPWM = TurnPID.Out;				//转向环的输出值给到电机差分PWM，用于控制左右转弯
      }
    }
  }
  TimerCount = __HAL_TIM_GET_COUNTER(&htim1); // 中断函数退出前，读取计数器的值，此值可用于测量中断函数的具体执行时间
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
