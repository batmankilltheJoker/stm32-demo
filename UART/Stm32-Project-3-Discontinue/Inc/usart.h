/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */
#define RXBUFFERSIZE 1024				//最大接收字节数
extern char Uart1RxBuffer[RXBUFFERSIZE];		//接收数据
extern uint8_t Uart1aRxBuffer;				//接收中断缓冲
extern uint16_t Uart1_Rx_Cnt ;			//接收缓冲计数

extern char Uart2RxBuffer[RXBUFFERSIZE];		//接收数据
extern uint8_t Uart2aRxBuffer;				//接收中断缓冲
extern uint16_t Uart2_Rx_Cnt ;			//接收缓冲计数
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

