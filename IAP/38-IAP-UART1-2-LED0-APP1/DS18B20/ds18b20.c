#include "ds18b20.h"
#include "gpio.h"
#include "tim.h"


uint8_t fg=1;
//���ģʽ
void OUTPUT_INIT_CONFIG()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DS18B20_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DS18B20_GPIO_Port, &GPIO_InitStruct);
}
//����ģʽ
void INPUT_INIT_CONFIG()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DS18B20_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DS18B20_GPIO_Port, &GPIO_InitStruct);
}
//��ʼ��
void Init_DS18B20(void)
{
	  OUTPUT_INIT_CONFIG();
		HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_SET); 
		delay_us(3);
		HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_RESET); 
		delay_us(750);
		HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_SET); 
		delay_us(40);
	  INPUT_INIT_CONFIG();
		if(HAL_GPIO_ReadPin(DS18B20_GPIO_Port,DS18B20_Pin)==GPIO_PIN_SET)
		{
			HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);//led��ָʾ
			delay_us(10);
		}
		else 
		{
			 OUTPUT_INIT_CONFIG();
			delay_us(480);
			HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_SET);
		}
		HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_SET);
		delay_us(100);
		fg=0;
}
//��ȡһ��һ�����ݣ��ֽڣ�
uint8_t ReadOneChar(void)
{
		unsigned char i=0; 
		uint8_t dat=0;
		for (i=8;i>0;i--) 
		{
			OUTPUT_INIT_CONFIG();
			HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_SET); 
			delay_us(3);
			HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_RESET);
			delay_us(3);
			dat>>=1;
			HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_SET);
			delay_us(5);
			INPUT_INIT_CONFIG();
			if(HAL_GPIO_ReadPin(DS18B20_GPIO_Port,DS18B20_Pin)==GPIO_PIN_SET)
				dat|=0x80;
			delay_us(60);
		}
		return(dat);
}
//д��һ��һ�����ݣ��ֽڣ�
void WriteOneChar(unsigned char dat)
{
	OUTPUT_INIT_CONFIG();
	unsigned char i=0; 
	for(i=8;i>0;i--) 
	{
		HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_RESET);
		delay_us(15);
		if(dat&0x01)
			HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_RESET);
		delay_us(45);
		HAL_GPIO_WritePin(DS18B20_GPIO_Port,DS18B20_Pin,GPIO_PIN_SET);
		dat>>=1;
	}
	delay_us(70);
}
//��ȡ����
float ReadTemperature(void)
{
	float mdata = 0;
	uint8_t tempL=0,tempH=0; 	
 	
	Init_DS18B20(); 
	WriteOneChar(0xcc); 
	WriteOneChar(0x44); 
	delay_us(500);
	Init_DS18B20(); 
	WriteOneChar(0xcc); 
	WriteOneChar(0xbe); 
	tempL=ReadOneChar(); //���ֽ�
	tempH=ReadOneChar(); //���ֽ�
	if(tempH>0x7f) 
	{
		tempL=~tempL; 
		tempH=~tempH+1;
		fg=1; 
	}
	else
		fg=0;
	mdata = tempL/16.0+tempH*16; 
	if(fg==1)
	{
		mdata=-mdata;
	}
	return mdata;
}

