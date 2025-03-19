/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
extern char ss[1000];
extern char IDstr[20];
int buildJsonData(char *ss);			//构建cjson格式,传输数据
//int AnalysisJsonData(char *ss);	//接收数据
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADC1PIN_Pin GPIO_PIN_6
#define ADC1PIN_GPIO_Port GPIOA
#define DHT11_Pin GPIO_PIN_6
#define DHT11_GPIO_Port GPIOC
#define Relay_Pin GPIO_PIN_9
#define Relay_GPIO_Port GPIOC
#define LCD_CLK_Pin GPIO_PIN_3
#define LCD_CLK_GPIO_Port GPIOB
#define LCD_SDI_Pin GPIO_PIN_4
#define LCD_SDI_GPIO_Port GPIOB
#define LCD_RST_Pin GPIO_PIN_5
#define LCD_RST_GPIO_Port GPIOB
#define LCD_RS_Pin GPIO_PIN_6
#define LCD_RS_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_7
#define LCD_CS_GPIO_Port GPIOB
#define LCD_LED_Pin GPIO_PIN_8
#define LCD_LED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
int buildJsonData(char *ss);//构建发布数据格式//
int AnalysisJsonData(char *ss);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
