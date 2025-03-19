/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "cJSON.h"


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
uint16_t ADC1Val=1000;//假设为1000
uint16_t PWMVal=0;
uint16_t X=1000;//个人的舒适值//后通过串口发送数据

char ss[1000]="";
char IDstr[20]="222154101127";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int fputc(int ch,FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t *) &ch,1,0xfff);
	return ch;
}

/*{
	"Id":	"222154101127",
	"Devices":	{
		"ADC1 Reading":"",
		"PA6 True Voltage value":"",
		"PWMVal":""
	}
}*/

int buildJsonData(char *ss)//构建数据格式
{
	char *cjson_print_buf;
	int len=0;
	cJSON *reported_obj;
	cJSON *device_obj;
	
	reported_obj=cJSON_CreateObject();
	device_obj=cJSON_CreateObject();
	
	cJSON_AddStringToObject(device_obj,"ID",IDstr);
	cJSON_AddNumberToObject(reported_obj,"ADC1 Reading",ADC1Val);
	
	cJSON_AddNumberToObject(reported_obj,"PA6 True Voltage value",ADC1Val*3.3f/4096);
	
	cJSON_AddNumberToObject(reported_obj,"PWMVal",PWMVal);
	cJSON_AddItemToObject(device_obj,"Devices",reported_obj);
	
	cjson_print_buf=cJSON_Print(device_obj);
	if(cjson_print_buf!=NULL)
	{
		len=snprintf(ss,2000,"%s",cjson_print_buf);
		cJSON_free(cjson_print_buf);
	}
	cJSON_Delete(device_obj);
	
	return len;
}

void LightCalculate()//计算光强
{
		HAL_ADC_Start(&hadc1);	//Start ADC
		HAL_ADC_PollForConversion(&hadc1,50);//轮询方式，超时50ms等待转化完成
		if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1),HAL_ADC_STATE_REG_EOC))//判断是否转化成功
		{
			ADC1Val = HAL_ADC_GetValue(&hadc1);//读出数据
//			printf("ADC1 Reading ：%d \r\n",ADC1Val);
//			printf("PA6 True Voltage value ：%.4f \r\n",ADC1Val*3.3f/4096);
		}
		if(ADC1Val>X)//X越大，亮度越暗
		{
			PWMVal=(ADC1Val-X)*1000/(4096-X);//假设数学模型为直线变化
		}
		else
			PWMVal = 0;
//		printf("PWMVal = %d \r\n",PWMVal);
		__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,PWMVal);//释放交流电
}

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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_ADCEx_Calibration_Start(&hadc1);//打开ADC1，进行数模转化
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);//打开定时器4，pwm方式
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2,Uart2_rx_buffer,BUF_SIZE);//haurt2
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		HAL_Delay(1000);
		LightCalculate();
		
		buildJsonData(ss);
		HAL_UART_Transmit(&huart2,(uint8_t *)ss,strlen(ss),0xffff);	//将接收到的数据再发回上位机
		HAL_UART_Transmit(&huart1,(uint8_t *)ss,strlen(ss),0xffff);
		memset(ss,0,1000);//清空数据
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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

#ifdef  USE_FULL_ASSERT
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
