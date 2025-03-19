/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x.h"

#include "Lcd_Driver.h"
#include "GUI.h"
#include "tim.h"
#include "Picture.h"
#include "QDTFT_demo.h"

#include <math.h>
#include <string.h>
#include <string.h>

#define PI 3.1415926

unsigned char Num[10]={0,1,2,3,4,5,6,7,8,9};
void Redraw_Mainmenu(void)
{

	Lcd_Clear(GRAY0);
	
	Gui_DrawFont_GBK16(16,3,BLUE,GRAY0,"瑞仿电子技术");
	Gui_DrawFont_GBK16(16,23,RED,GRAY0,"液晶测试程序");

	DisplayButtonUp(15,38,113,58); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(16,43,YELLOW,GRAY0,"颜色填充测试");

	DisplayButtonUp(15,68,113,88); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(16,73,BLUE,GRAY0,"文字显示测试");

	DisplayButtonUp(15,98,113,118); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(16,103,RED,GRAY0,"图片显示测试");
	
//	DisplayButtonUp(15,128,113,118); //x1,y1,x2,y2
//	Gui_DrawFont_GBK16(16,133,RED,GRAY0,"图片显示测试");
	HAL_Delay(10);//delay_ms(1500);
}

void Num_Test(void)
{
	uint8_t i=0;
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,20,RED,GRAY0,"Num Test");
	HAL_Delay(1000);//delay_ms(1000);
	Lcd_Clear(GRAY0);

	for(i=0;i<10;i++)
	{
		Gui_DrawFont_Num32((i%3)*40,32*(i/3)+5,RED,GRAY0,Num[i+1]);
		HAL_Delay(100);//delay_ms(100);
	}
	
}

void Font_Test(void)
{
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,10,BLUE,GRAY0,"文字显示测试");

	HAL_Delay(1000);//delay_ms(1000);
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,20,YELLOW,GRAY0,"瑞仿电子技术");
	Gui_DrawFont_GBK16(16,40,BLUE,GRAY0,"专注液晶批发");
	Gui_DrawFont_GBK16(16,60,RED,GRAY0, "全程技术支持");
	Gui_DrawFont_GBK16(5,80,BLUE,GRAY0,"Tel:13523436590");
	Gui_DrawFont_GBK16(5,100,RED,GRAY0, "QQ:1045913024");	
	HAL_Delay(1800);//delay_ms(1800);	
}

void Color_Test(void)
{
	uint8_t i=1;
	Lcd_Clear(GRAY0);
	
	Gui_DrawFont_GBK16(20,10,BLUE,GRAY0,"Color Test");
	HAL_Delay(200);//delay_ms(200);

	while(i--)
	{
		Lcd_Clear(WHITE);
		Lcd_Clear(BLACK);
		Lcd_Clear(RED);
	  Lcd_Clear(GREEN);
	  Lcd_Clear(BLUE);
	}		
}

//取模方式 水平扫描 从左到右 低位在前
void showimage(const unsigned char *p) //显示40*40 QQ图片
{
  	int i,j,k; 
	unsigned char picH,picL;
	Lcd_Clear(WHITE); //清屏  
	
	for(k=0;k<3;k++)
	{
	   	for(j=0;j<3;j++)
		{	
			Lcd_SetRegion(40*j,40*k,40*j+39,40*k+39);		//坐标设置
		    for(i=0;i<40*40;i++)
			 {	
			 	picL=*(p+i*2);	//数据低位在前
				picH=*(p+i*2+1);				
				LCD_WriteData_16Bit(picH<<8|picL);  						
			 }	
		 }
	}		
}

void Draw_SinCurve(void)
{
//	uint8_t CurveArr[128],i;
//	for(i=0;i<128;i++)
//		CurveArr[i]=255-(uint8_t)(127*sin(2*PI/128*i));
//	for(i=0;i<128;i++)
//		Gui_DrawPoint(i,CurveArr[i],GREEN);
	Gui_DrawLine(0,0,127,127,GREEN); 
}
	
void QDTFT_Test_Demo(void)
{
	Lcd_Init();
	HAL_GPIO_WritePin(LCD_LED_GPIO_Port, LCD_LED_Pin, GPIO_PIN_SET);//LCD_LED_SET;//通过IO控制背光亮				
	Redraw_Mainmenu();//绘制主菜单(部分内容由于分辨率超出物理值可能无法显示)
	Color_Test();//简单纯色填充测试
	Num_Test();//数码管字体测试
	Font_Test();//中英文显示测试		
	showimage(gImage_qq);//图片显示示例
	Draw_SinCurve();
	HAL_Delay(1200);//delay_ms(1200);
	HAL_GPIO_WritePin(LCD_LED_GPIO_Port, LCD_LED_Pin, GPIO_PIN_RESET);//LCD_LED_CLR;//IO控制背光灭	
}
