/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
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
#include "tim.h"

/* USER CODE BEGIN 0 */
#include"stdio.h"
/* USER CODE END 0 */

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim7;

/* TIM2 init function */
void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}
/* TIM7 init function */
void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 71;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 65535;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* tim_icHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(tim_icHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM2 GPIO Configuration
    PA1     ------> TIM2_CH2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM7)
  {
  /* USER CODE BEGIN TIM7_MspInit 0 */

  /* USER CODE END TIM7_MspInit 0 */
    /* TIM7 clock enable */
    __HAL_RCC_TIM7_CLK_ENABLE();
  /* USER CODE BEGIN TIM7_MspInit 1 */

  /* USER CODE END TIM7_MspInit 1 */
  }
}

void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef* tim_icHandle)
{

  if(tim_icHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /**TIM2 GPIO Configuration
    PA1     ------> TIM2_CH2
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);

    /* TIM2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM7)
  {
  /* USER CODE BEGIN TIM7_MspDeInit 0 */

  /* USER CODE END TIM7_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM7_CLK_DISABLE();
  /* USER CODE BEGIN TIM7_MspDeInit 1 */

  /* USER CODE END TIM7_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
COUNT_TEMP IC_Count_Arr[5]={0};
uint8_t IC_Count_Index=0;
uint32_t Tim2Count=0;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	int temp=0,temp2=0;
	
	if(TIM2 == htim->Instance)//判断定时器2
	{
		if(IC_Count_Arr[IC_Count_Index].flg == 0)
		{
			//清零定时器计数
			IC_Count_Arr[IC_Count_Index].num[0]=__HAL_TIM_GET_COUNTER(htim);
			//设置上沿触发
			__HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_FALLING);
			
			IC_Count_Arr[IC_Count_Index].flg = 1;			//设置已经开始
			IC_Count_Arr[IC_Count_Index].num_period[0] = 0;		//溢出计数清零
			IC_Count_Arr[IC_Count_Index].num_period[1] = 0;
			temp=IC_Count_Index==0?4:IC_Count_Index-1;
			temp2=temp==0?4:temp-1;
			printf("%u\r\n%u\r\n",
				(IC_Count_Arr[temp].num_period[1]*5000+IC_Count_Arr[temp].num[1]-IC_Count_Arr[temp].num_period[0]*5000+IC_Count_Arr[temp].num[0]),
				(IC_Count_Arr[temp].num_period[0]*5000+IC_Count_Arr[temp].num[0]-IC_Count_Arr[temp2].num_period[1]*5000+IC_Count_Arr[temp2].num[1]));
		}
		else
		{
				//获取定时器计数值
			IC_Count_Arr[IC_Count_Index].num[1] = HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_2);
			IC_Count_Arr[IC_Count_Index].num_period[1] = 0;
				//设置下降沿触发
			IC_Count_Index++;
			IC_Count_Index%=5;
			__HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);
			IC_Count_Arr[IC_Count_Index].flg = 0;
			
		}
	}
}
//定时器溢出回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	uint8_t temp=0;
	if(TIM2 == htim->Instance)
	{
		Tim2Count++;
		//每次溢出时间为5000us
		if(IC_Count_Arr[IC_Count_Index].flg==1)//还未成功捕获
		{
			if(IC_Count_Arr[IC_Count_Index].num_period[0]==0XFFFF)//电平太长了
			{
				IC_Count_Arr[IC_Count_Index].flg=0;					//标记成功捕获一次
				IC_Count_Arr[IC_Count_Index].num[0]=0xffff;
			}
			else 
				IC_Count_Arr[IC_Count_Index].num_period[0]=Tim2Count;
		}
		
		else
			{
				temp=IC_Count_Index==0?4:IC_Count_Index-1;
				
			if(IC_Count_Arr[IC_Count_Index].flg==1)//还未成功捕获
		{
			if(IC_Count_Arr[IC_Count_Index].num_period[0]==0XFFFF)//电平太长了
			{
				IC_Count_Arr[IC_Count_Index].flg=0;					//标记成功捕获一次
				IC_Count_Arr[IC_Count_Index].num[1] =0xffff;
			}
			else 
				IC_Count_Arr[IC_Count_Index].num_period[1]= Tim2Count;
			}
		}
	}
}
/* USER CODE END 1 */
