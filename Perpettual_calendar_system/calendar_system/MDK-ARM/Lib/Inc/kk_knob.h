#ifndef INC_KK_KNOB_H_
#define INC_KK_KNOB_H_

#include "tim.h"

/**
	* typedef ����һ������
	* void ��������ֵΪvoid
	* (*KnobCallback) ������ΪKnobCallback����Ϊ������Ǻ���ָ�����͹ʼ� * ��
	* (void) ��������
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
