/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
typedef struct 
{
	uint8_t		flg;											//0为未开始，1为开始，2为结束//
	uint16_t		num_cnt;								//计数值//
	uint16_t		num_period;							//溢出次数//
}COUNT_TEMP;
extern COUNT_TEMP IC_Count;
extern int distance;
/* USER CODE END Includes */

extern TIM_HandleTypeDef htim4;

extern TIM_HandleTypeDef htim7;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_TIM4_Init(void);
void MX_TIM7_Init(void);

/* USER CODE BEGIN Prototypes */
void delay_us(uint16_t us);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

