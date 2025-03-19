#include "dht11.h"
#include "tim.h"
//���ģʽ��ʼ��
void DHT11_OUTPUT_INIT_CONFIG()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}
//����ģʽ��ʼ��
void DHT11_INPUT_INIT_CONFIG()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}
//һ��һ����ȡ�ֽڣ���8bit
uint8_t ReadByte(void)
{
	uint8_t i,FLAG,temp,dat;
	for(i=0;i<8;i++)
	{
		FLAG=2;//����״̬
		DHT11_INPUT_INIT_CONFIG();//����Ϊ����ģʽ
		while((HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==GPIO_PIN_RESET)&&FLAG++);//�ȴ���Ϊ�͵�ƽ
		delay_us(30);//��ʱ30us
		temp=0;//�ı�״̬
		if(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==GPIO_PIN_SET)//�ٶ�ȡ
			temp=1;
		FLAG=2;
		dat<<=1;
		dat|=temp; //�����Ϊ��ΪDHT11��״̬���͵�ƽ���Ǹߵ�ƽ
		while(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==GPIO_PIN_SET);
	}
	return dat;
}
//��ʼ���
void DHT11ReadHumiture(uint8_t Arr[])
{
	uint8_t temp,FLAG,i;
	uint8_t Humidity_H,Humidity_L,Temperature_H,Temperature_L,CheckByte;
	DHT11_OUTPUT_INIT_CONFIG();//���ģʽ
	HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin,GPIO_PIN_RESET);
	for(i=0;i<18;i++)
		delay_us(1000);//��ʱ18ms
	DHT11_INPUT_INIT_CONFIG();//����ģʽ
	delay_us(20);//����20us
	if(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==GPIO_PIN_RESET)
	{
		DHT11_OUTPUT_INIT_CONFIG();//����Ϊ�͵�ƽ������Ϊ���ģʽ
	}
	DHT11_INPUT_INIT_CONFIG();//�ߵ�ƽ������ģʽ
	if(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==GPIO_PIN_RESET) 
	{
		//����
		FLAG=2;
		while((HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==GPIO_PIN_RESET)&&FLAG++);
		FLAG=2;
		while((HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)==GPIO_PIN_SET)&&FLAG++);
		//���ֽ�Ϊ���������ֽ�ΪС��
		Humidity_H=ReadByte();//ʪ����������
		Humidity_L=ReadByte();//ʪ��С������
		Temperature_H=ReadByte();//�¶���������
		Temperature_L=ReadByte();//�¶�С������
		CheckByte=ReadByte();//��ȡУ����
		//��������ȡ5���ֽ�
		DHT11_OUTPUT_INIT_CONFIG();//����Ϊ���ģʽ
		HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin,GPIO_PIN_SET);
		temp=(Humidity_H+Humidity_L+Temperature_H+Temperature_L);//����У����
		if(temp==CheckByte)//������ɵ�У����Ͷ�ȡ��У������ͬ
		{
			Arr[0]=Humidity_H;//ʪ����������
			Arr[1]=Humidity_L;//ʪ��С������
			Arr[2]=Temperature_H;//�¶���������
			Arr[3]=Temperature_L;//�¶�С������
			Arr[4]=CheckByte;//У�鲿��
		}
	}
	
}












