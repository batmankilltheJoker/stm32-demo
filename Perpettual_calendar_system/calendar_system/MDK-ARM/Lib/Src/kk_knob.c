#include "kk_knob.h"
/**
	* 设置计数器为初始值 为65535的一半，尽可能远离计数器最大与最小的两端，防止溢出
	* 获取计数器的值
	* 计数器值 VS 初始值
	*		大于：说明旋钮顺时针旋转 -> 执行正转任务
	*		小于：说明旋钮逆时针旋转 -> 执行反转任务
	* 	相等：说明旋钮没有旋转   -> 无任务执行
	* 复位计数器值，方便下一次判断
	* 判断旋钮是否被按下
 */

#define COUNTER_INIT_VALUE 65535/2		//计数器初始值
#define BTN_DEBOUNCE_TICKS 10					//按键消抖时长

typedef enum {
	Pressed, 
	Unpressed
} BtnState;		//枚举类型，定义按键的状态，类型名为BtnState

/**
	* 旋钮正转时的回调函数
	* 执行正转任务
		* 设置指针 void Knob_SetForwardCallback(KnobCallback callback)
	*/
KnobCallback onForwardCallback = NULL;

void Knob_SetForwardCallback(KnobCallback callback){
	onForwardCallback = callback;
}


/**
	* 旋钮反转时的回调函数
	* 执行反转任务
	* 设置指针 void Knob_SetBackwardCallback(KnobCallback callback)
	*/
KnobCallback onBackwardCallback = NULL;

void Knob_SetBackwardCallback(KnobCallback callback){
	onBackwardCallback = callback;
}


/**
	* 旋钮按下时的回调函数
	* 执行按下任务
	* 设置指针 void Knob_SetPressedCallback(KnobCallback callback)
	*/
KnobCallback onPressedCallback = NULL;

void Knob_SetPressedCallback(KnobCallback callback){
	onPressedCallback = callback;
}


/**
	* 编码器的初始化函数
	*/
void Knob_Init(void){
	starEncoder();									// 开启遍码器通道
	setCounter(COUNTER_INIT_VALUE);	// 设置计数器初始值
}

/**
	* 开启遍码器通道
	* 由于此操作与硬件层耦合，故包装为函数
	*/
void starEncoder(void){
	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
}

/**
	* 设置计数器初始值
	* 由于此操作与硬件层耦合，故包装为函数
	*/
void setCounter(int value){
	__HAL_TIM_SET_COUNTER(&htim1, value);
}


/**
	* 获取计数器值
	* 由于此操作与硬件层耦合，故包装为函数
	*/
uint32_t getCounter(void){
	return __HAL_TIM_GET_COUNTER(&htim1);
}


/**
	* 获取按键状态
	* 即旋钮是否被按下
	* 此处只读取了IO口的电平，但没有进行消抖！！！
	*/
BtnState getBtnState(){
	return HAL_GPIO_ReadPin(KnobBtn_GPIO_Port, KnobBtn_Pin) == GPIO_PIN_RESET ? Pressed : Unpressed;
}


/**
	* 获得当前时间
	* 返回当前系统已经运行的ms数
	* 由于此操作与硬件层耦合，故包装为函数
	*/
uint32_t getTick(){
	return HAL_GetTick();
}

/**
	* 对计数器值和初始值进行比较判断
	*/
void Knob_Loop(void){
	uint32_t counter = getCounter();
	
	if (counter > COUNTER_INIT_VALUE){
		/* 执行正转任务 */
		if(onForwardCallback != NULL){	// 判断onForwardCallback是否被赋值
			onForwardCallback();
		}
		
	}else if(counter < COUNTER_INIT_VALUE){
		/* 执行反转任务 */
		if(onBackwardCallback != NULL){	// 判断onBackwardCallback是否被赋值
			onBackwardCallback();
		}
	}
	setCounter(COUNTER_INIT_VALUE);		// 对计数器进行复位
	
	/** 
		* 由于getBtnState()没有进行消抖10ms
		* 但直接进行延时10ms会造成整个程序的所有任务都被堵塞 
		*/
	BtnState btnState = getBtnState();	// 当前按键状态
	static uint8_t callbackState = 0;		// 记录本次按键事件中，回调函数是否调用 0:未调用 1:已调用
	static uint32_t pressedTime = 0;
	
	if (btnState == Pressed){
		if (pressedTime == 0){
			pressedTime = getTick();				//记录按下的第一时间
		}else if (callbackState == 0 && getTick() - pressedTime > BTN_DEBOUNCE_TICKS){		// 当前时刻与按下时刻的差值即消抖后，按键稳定处于按下的时间
			// 按键消抖完成 且未执行回调函数
			if (onPressedCallback != NULL){
				onPressedCallback();	//执行回调函数
			}
			callbackState = 1;			//标志已调用回调函数 且避免重复调用回调函数
		}
	}else {
		// 按键松开
		pressedTime = 0;
		callbackState = 0;
	}
}



