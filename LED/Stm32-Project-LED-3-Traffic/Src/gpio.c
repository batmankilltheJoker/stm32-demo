/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, EWG_Pin|EWR_Pin|EWY_Pin|SNG_Pin
                          |SNR_Pin|SNY_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PAPin PAPin PAPin PAPin
                           PAPin PAPin */
  GPIO_InitStruct.Pin = EWG_Pin|EWR_Pin|EWY_Pin|SNG_Pin
                          |SNR_Pin|SNY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
uint8_t traffic_lightCtrls(uint8_t CtrolCode) 
{
	if(CtrolCode-16==0)
	{
		HAL_GPIO_WritePin(EWG_GPIO_Port, EWG_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SNR_GPIO_Port, SNR_Pin, GPIO_PIN_RESET);
		HAL_Delay(2000);
	}
	else{
		HAL_GPIO_WritePin(EWG_GPIO_Port, EWG_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(SNR_GPIO_Port, SNR_Pin, GPIO_PIN_SET);
	}
	
	if(CtrolCode-54==0){
		HAL_GPIO_WritePin(EWY_GPIO_Port, EWY_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SNY_GPIO_Port, SNY_Pin, GPIO_PIN_RESET);
	}
	else {
		HAL_GPIO_WritePin(EWY_GPIO_Port, EWY_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(SNY_GPIO_Port, SNY_Pin, GPIO_PIN_SET);
	}
	
	if(CtrolCode-43==0){
		HAL_GPIO_WritePin(EWR_GPIO_Port, EWR_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SNG_GPIO_Port, SNG_Pin, GPIO_PIN_RESET);
		HAL_Delay(2000);
	}
	else {
		HAL_GPIO_WritePin(EWR_GPIO_Port, EWR_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(SNG_GPIO_Port, SNG_Pin, GPIO_PIN_SET);
	}
	return 0;
}

uint8_t traffic_lightsArray[4]={0x10,0x36,0x2B,0x36};

uint8_t traffic_lightsCtrl(uint8_t LED_Index,uint16_t n_delay_ms)
{
	traffic_lightCtrls(traffic_lightsArray[LED_Index]);
	HAL_Delay(n_delay_ms);
	return 0;
}
/* USER CODE END 2 */
