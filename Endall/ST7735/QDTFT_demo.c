/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x.h"

#include "Lcd_Driver.h"
#include "GUI.h"
#include "tim.h"
#include "Picture.h"
#include "QDTFT_demo.h"

#include <math.h>
#include <string.h>

#include "stdio.h"
#include "dht11.h"
#include "stdlib.h"
#include "mq135.h"

#define PI 3.1415926

double hum = 0;
double tem = 0;

uint8_t DHT_Values[5] = {0}; 
char AIR[10];
char humidity[10];
char temperature[10];
char temp_str[10];

void DHT11ReadHumiturePrint(void)
{
	HAL_Delay(1000);//delay_ms(1000);
	Lcd_Clear(GRAY0);
	
	//	AirqualityCalculatePrint
	Gui_DrawFont_GBK16(1,20,RED,GRAY0, "Airquality:");	
	sprintf(AIR,"%lf",ad);
	Gui_DrawFont_GBK16(1,40,BLUE,GRAY0, (uint8_t *)AIR);	
	
//	Gui_DrawFont_GBK16(10,20,RED,GRAY0, "DHT11 Testing:");	
	
	DHT11ReadHumiture(DHT_Values);
	
	//����ֵת��Ϊ�ַ������
	sprintf(humidity,"%d.%d",(uint8_t)DHT_Values[0],(uint8_t)DHT_Values[1]);			// ������ֵ��ʽ��Ϊ�ַ���
	DHT_Values[0] = atof(humidity);
	hum = (double)DHT_Values[0];//���ַ���ת����ʵ�������棬����cjson����
	
	sprintf(temperature,"%d.%d",(uint8_t)DHT_Values[2],(uint8_t)DHT_Values[3]);		// ������ֵ��ʽ��Ϊ�ַ���
	DHT_Values[2] = atof(temperature);
	tem = (double)DHT_Values[2];//���ַ���ת����ʵ�������棬����cjson����

	
	Gui_DrawFont_GBK16(1,60,BLUE,GRAY0,"humidity:");
	Gui_DrawFont_GBK16(1,80,BLUE,GRAY0,(uint8_t *)humidity);
	Gui_DrawFont_GBK16(1,100,RED,GRAY0,"temperature:");
	Gui_DrawFont_GBK16(1,120,BLUE,GRAY0,(uint8_t *)temperature);
	
}
void QDTFT_Test_Demo(void)
{
	Lcd_Init();
	HAL_GPIO_WritePin(LCD_LED_GPIO_Port, LCD_LED_Pin, GPIO_PIN_SET);//LCD_LED_SET;//ͨ��IO���Ʊ�����				
	DHT11ReadHumiturePrint();		//DHT11��ʪ����ʾ
	HAL_Delay(1200);//delay_ms(1200);
	HAL_GPIO_WritePin(LCD_LED_GPIO_Port, LCD_LED_Pin, GPIO_PIN_RESET);//LCD_LED_CLR;//IO���Ʊ�����	
}
