/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "CommTask.h"		/* 蓝牙队列 BlueRxQueue 和 BluePacket_t 的归属模块 */
/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TX_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(RX_GPIO_Port, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = TXD_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TXD_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = RXD_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(RXD_GPIO_Port, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, TX_Pin|RX_Pin);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, TXD_Pin|RXD_Pin);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
static uint8_t RxData1;		         // 定义串口接收临时缓冲区,加个static使其在其他文件重复命名而不干扰
uint8_t Serial_RxData;             // USART接收到的数据字节
uint8_t Serial_RxFlag;             // USART接收完成标志：1代表收到新数据，主循环读取后清零

static uint8_t RxData2;		         // 定义串口接收临时缓冲区，加个static使其在其他文件重复命名而不干扰
uint8_t BlueSerial_RxData;		     // 定义蓝牙串口接收的数据字节

/*
 * 该函数用于串口1接收开始
 * 参数：无
 * 返回值：无
 */
void UART1_Receive_Start(void)
{
  HAL_UART_Receive_IT(&huart1, &RxData1, 1);
}

/*
 * 该函数用于串口2接收开始
 * 参数：无
 * 返回值：无
 */
void UART2_Receive_Start(void)
{
  HAL_UART_Receive_IT(&huart2, &RxData2, 1);
}

/*
 * 串口1和串口2接收中断回调函数
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  static uint8_t RxState = 0;		   //定义表示当前状态机状态的静态变量
  static uint8_t pRxPacket = 0;	   //定义表示当前接收数据位置的静态变量
  if(huart->Instance == USART1)
  {
    Serial_RxData = RxData1;     //将HAL临时缓存里面收到的1字节数据拷贝到全局接收变量
    Serial_RxFlag = 1;          //置接收完成标志位，通知主循环：已经收到串口1的数据

    //❗重新提交接收任务，持续等待下一字节（HAL必须！标准库不需要）
    HAL_UART_Receive_IT(huart, &RxData1, 1);
  }
  else if(huart->Instance == USART2)
  {
    //标准库中的 USART_ReceiveData(USART2) 已被Hal库库函数封装（等价于HAL_UART_Receive_IT(huart, &RxData2, 1);）
    static char Buf[BLUE_PACKET_LEN];	//数据包组装缓冲区，只在本回调（中断上下文）里用，不再和任务共享
    uint8_t Blue_RxData = RxData2;
    /* if和RxState 起到的作用是规定顺序，先接收到包头，再开始准备接收数据，最后接收到']'结束
     * pRxPacket   同理，用于规定数据的接收顺序
     */
    if (RxState == 0)		//当前状态为0，接收数据包包头
    {
      if (Blue_RxData == '[')	//如果数据确实是包头
      {
        RxState = 1;			//置下一个状态
        pRxPacket = 0;			//数据包的位置归零
      }
    }
    else if (RxState == 1)	//当前状态为1，接收数据包数据，同时判断是否接收到了包尾
    {
      if (Blue_RxData == ']')			//如果收到包尾
      {
        RxState = 0;			//状态归0
        Buf[pRxPacket] = '\0';	//将收到的字符数据包添加一个字符串结束标志
        /*交给 commTask 处理：投队列；超时 0 表示队列满了就丢掉这一包。
         *蓝牙中断可能早于队列创建（UART2_Receive_Start 在 MX_FREERTOS_Init 之前），故判 NULL*/
        if (BlueRxQueue != NULL)
        {
          osMessageQueuePut(BlueRxQueue, Buf, 0, 0);
        }
      }
      else						//接收到了正常的数据
      {
        Buf[pRxPacket] = Blue_RxData;	//将数据存入数据包数组的指定位置
        if (pRxPacket < BLUE_PACKET_LEN - 1) pRxPacket ++;			//数据包的位置自增（到上限不再增长，防止越界）
      }
    }
    //❗HAL库必须重新提交接收任务，继续等待下一字节；标准库不需要这一句
    HAL_UART_Receive_IT(huart, &RxData2, 1);
  }
}
/* USER CODE END 1 */
