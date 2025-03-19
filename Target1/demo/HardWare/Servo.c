#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Servo_Init()
{
	PWM_Init();	
}
	/*舵机位置
	    1        2
	
	

	
	    3        4
	*/
	
	//舵机充当腿，括号里的值为0时表示的是腿向前进的方向甩，90度是站立
void Servo_Angle1(float Angle)//左上
{
	PWM_SetCompare1(Angle / 180 * 2000 + 500);			
}

void Servo_Angle2(float Angle)//右上
{
	PWM_SetCompare2((180-Angle) / 180 * 2000 + 500);		
}

void Servo_Angle3(float Angle)//左下
{
	PWM_SetCompare3(Angle / 180 * 2000 + 500);			
}

void Servo_Angle4(float Angle)//右下
{
	PWM_SetCompare4((180-Angle) / 180 * 2000 + 500);			
}

void WServo_Angle(float Angle)//尾巴
{
	PWM_WSetCompare(Angle / 180 * 2000 + 500);			
}
