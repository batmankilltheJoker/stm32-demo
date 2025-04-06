#ifndef INC_KK_KNOB_H_
#define INC_KK_KNOB_H_

#include "tim.h"

/**
	* typedef 定义一个类型
	* void 函数返回值为void
	* (*KnobCallback) 函数名为KnobCallback，因为定义的是函数指针类型故加 * 号
	* (void) 函数参数
	*/
typedef void (*KnobCallback)(void);
void Knob_SetForwardCallback(KnobCallback callback);
void Knob_SetBackwardCallback(KnobCallback callback);
void Knob_SetPressedCallback(KnobCallback callback);

void Knob_Init(void);
void starEncoder(void);
void setCounter(int value);
uint32_t getCounter(void);
void Knob_Loop(void);

#endif /* INC_KK_KNOB_H_ */
