#include "dht11.h"
#include "tim.h"
//输出模式初始化
void DHT11_OUTPUT_INIT_CONFIG()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}
//输入模式初始化
void DHT11_INPUT_INIT_CONFIG()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}
//一个一个读取字节，共8bit
uint8_t ReadByte(void)
{
	uint8_t i,FLAG,temp,dat;
	for(i=0;i<8;i++)
	{
		FLAG=2;//设置状态
		DHT11_INPUT_INIT_CONFIG();//设置为输入模式
		while((HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==GPIO_PIN_RESET)&&FLAG++);//等待变为低电平
		delay_us(30);//延时30us
		temp=0;//改变状态
		if(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==GPIO_PIN_SET)//再读取
			temp=1;
		FLAG=2;
		dat<<=1;
		dat|=temp; //将最低为改为DHT11的状态，低电平还是高电平
		while(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==GPIO_PIN_SET);
	}
	return dat;
}
//开始输出
void DHT11ReadHumiture(uint8_t Arr[])
{
	uint8_t temp,FLAG,i;
	uint8_t Humidity_H,Humidity_L,Temperature_H,Temperature_L,CheckByte;
	DHT11_OUTPUT_INIT_CONFIG();//输出模式
	HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin,GPIO_PIN_RESET);
	for(i=0;i<18;i++)
		delay_us(1000);//延时18ms
	DHT11_INPUT_INIT_CONFIG();//输入模式
	delay_us(20);//持续20us
	if(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==GPIO_PIN_RESET)
	{
		DHT11_OUTPUT_INIT_CONFIG();//若变为低电平，设置为输出模式
	}
	DHT11_INPUT_INIT_CONFIG();//高电平，输入模式
	if(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==GPIO_PIN_RESET) 
	{
		//启动
		FLAG=2;
		while((HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==GPIO_PIN_RESET)&&FLAG++);
		FLAG=2;
		while((HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==GPIO_PIN_SET)&&FLAG++);
		//高字节为整数，低字节为小数
		Humidity_H=ReadByte();//湿度整数部分
		Humidity_L=ReadByte();//湿度小数部分
		Temperature_H=ReadByte();//温度整数部分
		Temperature_L=ReadByte();//温度小数部分
		CheckByte=ReadByte();//读取校验码
		//上述共读取5个字节
		DHT11_OUTPUT_INIT_CONFIG();//配置为输出模式
		HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin,GPIO_PIN_SET);
		temp=(Humidity_H+Humidity_L+Temperature_H+Temperature_L);//计算校验码
		if(temp==CheckByte)//如果生成的校验码和读取的校验码相同
		{
			Arr[0]=Humidity_H;//湿度整数部分
			Arr[1]=Humidity_L;//湿度小数部分
			Arr[2]=Temperature_H;//温度整数部分
			Arr[3]=Temperature_L;//温度小数部分
			Arr[4]=CheckByte;//校验部分
		}
	}
	
}












