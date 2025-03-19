#include<mq135.h>
#include"adc.h"
#include"math.h"
#include"main.h"
#include"stdio.h"
//#include"tim.h"
double ADC1Val=0;//ADC采样值
double PWMVal=0;//实际电压值
double ad;//实际浓度

void AirqualityCalculate()//计算空气质量
{
		HAL_ADC_Start(&hadc1);	//Start ADC
		HAL_ADC_PollForConversion(&hadc1,50);//轮询方式，超时50ms等待转化完成
		if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1),HAL_ADC_STATE_REG_EOC))//判断是否转化成功
		{
			ADC1Val = HAL_ADC_GetValue(&hadc1);//读出数据(ADC采样值)
//			printf("\r\nADC1Val: %lf\n",ADC1Val);
		}
		
		PWMVal = ADC1Val*(3.3/4096);
//		printf("PWMVal: %lf\n",PWMVal);
		ad = pow((3.4880*10*PWMVal)/(5-PWMVal),(1.0/0.3203));//其中的5，来自于供电电压5v
//		ad = pow(11.5428 * 35.904 * PWMVal/(25.5-5.1* PWMVal),0.6549);
//		ad = ADC1Val * 99 / 4096.0;
//		printf("ad: %lf\r\n",ad);
}
//计算方式来自于：
//https://blog.csdn.net/weixin_38075894/article/details/111589375?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522170444298916800186518358%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=170444298916800186518358&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v31_ecpm-1-111589375-null-null.142^v99^pc_search_result_base1&utm_term=mq135%E7%A9%BA%E6%B0%94%E8%B4%A8%E9%87%8F%E4%BC%A0%E6%84%9F%E5%99%A8%E5%B0%86%E5%85%B6%E7%94%B5%E5%8E%8B%E5%80%BC%E8%BD%AC%E5%8C%96%E4%B8%BA%E5%AE%9E%E9%99%85%E5%80%BC&spm=1018.2226.3001.4187
