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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cJSON.h"
#include "stdio.h"
#include "string.h"
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

int buildJsonData(char *ss)//�������ݸ�ʽ
{
	char *cjson_print_buf;
	int len=0;
	cJSON *reported_obj;
	cJSON *device_obj;
	
	reported_obj=cJSON_CreateObject();
	device_obj=cJSON_CreateObject();
	
	cJSON_AddStringToObject(device_obj,"ID",IDstr);
	cJSON_AddNumberToObject(reported_obj,"LightIntensity",1057);
	
	cJSON_AddNumberToObject(reported_obj,"Temp",23.1);
	
	cJSON_AddNumberToObject(reported_obj,"Humidity",78.3);
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

/*{
	"Id":	"222154101127",
	"Devices":	{
		"LED1":		"ON",
		"BEEP":		"OFF",
		"MOTOR":	"OFF"
	}
}*/
int AnalysisJsonData(char *ss)//��������//
{
	int ret=0;
	cJSON *receive_obj;//������������
	cJSON *device_obj;//����������
	cJSON *item_obj;//��������
	char *str;
	receive_obj=cJSON_Parse((char *)ss);//ת��Ϊcjson��ʽ//
//	printf("A0\r\n");
	if(receive_obj==NULL)
	{
		return ret;		
	}
	device_obj=cJSON_GetObjectItem(receive_obj,"ID");//��ȡID��һ��//
//	printf("A1\r\n");
	if(strcmp(device_obj->valuestring,IDstr))//�Ƚ�ID,�����򷵻�//
	{
		ret=1;
		return ret;
	}
	device_obj=cJSON_GetObjectItem(receive_obj,"Devices");//��ȡDevices��һ��
//	printf("A2\r\n");
	item_obj=device_obj->child;//ָ��Devices�е�����//
	
	while(item_obj)//�ֱ�Ƚ�Devices�е�����:LED/BEEP/MOTOR
	{
		//printf("A3\r\n");
		str=item_obj->string;//��ȡ�������ַ���//
		if(!strcmp(str,"LED1"))//�ж��Ƿ�ΪLED1
		{
//			printf("AN\r\n");
			if(strcmp(item_obj->valuestring,"ON")==0)
			{
				HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);
				printf("LED1 ON\r\n");
				ret=2;
			}
			else if(strcmp(item_obj->valuestring,"OFF")==0)
			{
				HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET);
				printf("LED1 OFF\r\n");
				ret=3;
			}
		}
		if(!strcmp(str,"BEEP"))//����ͬ��//
		{
			if(strcmp(item_obj->valuestring,"ON")==0)
			{
				HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);//RESET
				printf("BEEP ON\r\n");
				ret=4;
			}
			else if(strcmp(item_obj->valuestring,"OFF")==0)
			{
				HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
				printf("BEEP OFF\r\n");
				ret=5;
			}
		}
		if(!strcmp(str,"MOTOR"))//����ͬ��//
		{
			if(strcmp(item_obj->valuestring,"ON")==0)
			{
				HAL_GPIO_WritePin(MOTOR_GPIO_Port,MOTOR_Pin,GPIO_PIN_SET);//RESET
				printf("MOTOR ON\r\n");
				ret=6;
			}
			else if(strcmp(item_obj->valuestring,"OFF")==0)
			{
				HAL_GPIO_WritePin(MOTOR_GPIO_Port,MOTOR_Pin,GPIO_PIN_RESET);
				printf("MOTOR OFF\r\n");
				ret=7;
			}
		}
		item_obj=item_obj->next;//��ȡ��һ������
	}
	cJSON_Delete(receive_obj);//�ͷ�����
	return ret;
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2,Uart2_rx_buffer,BUF_SIZE);//&huart2
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		buildJsonData(ss);
		HAL_UART_Transmit(&huart2,(uint8_t *)ss,strlen(ss),0xffff);	//�����յ��������ٷ�����λ��
		HAL_UART_Transmit(&huart1,(uint8_t *)ss,strlen(ss),0xffff);
		memset(ss,0,1000);//�������
		HAL_Delay(2000);
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
