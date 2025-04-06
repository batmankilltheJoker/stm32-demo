#include "kk_knob.h"
/**
	* ���ü�����Ϊ��ʼֵ Ϊ65535��һ�룬������Զ��������������С�����ˣ���ֹ���
	* ��ȡ��������ֵ
	* ������ֵ VS ��ʼֵ
	*		���ڣ�˵����ť˳ʱ����ת -> ִ����ת����
	*		С�ڣ�˵����ť��ʱ����ת -> ִ�з�ת����
	* 	��ȣ�˵����ťû����ת   -> ������ִ��
	* ��λ������ֵ��������һ���ж�
	* �ж���ť�Ƿ񱻰���
 */

#define COUNTER_INIT_VALUE 65535/2		//��������ʼֵ
#define BTN_DEBOUNCE_TICKS 10					//��������ʱ��

typedef enum {
	Pressed, 
	Unpressed
} BtnState;		//ö�����ͣ����尴����״̬��������ΪBtnState

/**
	* ��ť��תʱ�Ļص�����
	* ִ����ת����
		* ����ָ�� void Knob_SetForwardCallback(KnobCallback callback)
	*/
KnobCallback onForwardCallback = NULL;

void Knob_SetForwardCallback(KnobCallback callback){
	onForwardCallback = callback;
}


/**
	* ��ť��תʱ�Ļص�����
	* ִ�з�ת����
	* ����ָ�� void Knob_SetBackwardCallback(KnobCallback callback)
	*/
KnobCallback onBackwardCallback = NULL;

void Knob_SetBackwardCallback(KnobCallback callback){
	onBackwardCallback = callback;
}


/**
	* ��ť����ʱ�Ļص�����
	* ִ�а�������
	* ����ָ�� void Knob_SetPressedCallback(KnobCallback callback)
	*/
KnobCallback onPressedCallback = NULL;

void Knob_SetPressedCallback(KnobCallback callback){
	onPressedCallback = callback;
}


/**
	* �������ĳ�ʼ������
	*/
void Knob_Init(void){
	starEncoder();									// ����������ͨ��
	setCounter(COUNTER_INIT_VALUE);	// ���ü�������ʼֵ
}

/**
	* ����������ͨ��
	* ���ڴ˲�����Ӳ������ϣ��ʰ�װΪ����
	*/
void starEncoder(void){
	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
}

/**
	* ���ü�������ʼֵ
	* ���ڴ˲�����Ӳ������ϣ��ʰ�װΪ����
	*/
void setCounter(int value){
	__HAL_TIM_SET_COUNTER(&htim1, value);
}


/**
	* ��ȡ������ֵ
	* ���ڴ˲�����Ӳ������ϣ��ʰ�װΪ����
	*/
uint32_t getCounter(void){
	return __HAL_TIM_GET_COUNTER(&htim1);
}


/**
	* ��ȡ����״̬
	* ����ť�Ƿ񱻰���
	* �˴�ֻ��ȡ��IO�ڵĵ�ƽ����û�н�������������
	*/
BtnState getBtnState(){
	return HAL_GPIO_ReadPin(KnobBtn_GPIO_Port, KnobBtn_Pin) == GPIO_PIN_RESET ? Pressed : Unpressed;
}


/**
	* ��õ�ǰʱ��
	* ���ص�ǰϵͳ�Ѿ����е�ms��
	* ���ڴ˲�����Ӳ������ϣ��ʰ�װΪ����
	*/
uint32_t getTick(){
	return HAL_GetTick();
}

/**
	* �Լ�����ֵ�ͳ�ʼֵ���бȽ��ж�
	*/
void Knob_Loop(void){
	uint32_t counter = getCounter();
	
	if (counter > COUNTER_INIT_VALUE){
		/* ִ����ת���� */
		if(onForwardCallback != NULL){	// �ж�onForwardCallback�Ƿ񱻸�ֵ
			onForwardCallback();
		}
		
	}else if(counter < COUNTER_INIT_VALUE){
		/* ִ�з�ת���� */
		if(onBackwardCallback != NULL){	// �ж�onBackwardCallback�Ƿ񱻸�ֵ
			onBackwardCallback();
		}
	}
	setCounter(COUNTER_INIT_VALUE);		// �Լ��������и�λ
	
	/** 
		* ����getBtnState()û�н�������10ms
		* ��ֱ�ӽ�����ʱ10ms���������������������񶼱����� 
		*/
	BtnState btnState = getBtnState();	// ��ǰ����״̬
	static uint8_t callbackState = 0;		// ��¼���ΰ����¼��У��ص������Ƿ���� 0:δ���� 1:�ѵ���
	static uint32_t pressedTime = 0;
	
	if (btnState == Pressed){
		if (pressedTime == 0){
			pressedTime = getTick();				//��¼���µĵ�һʱ��
		}else if (callbackState == 0 && getTick() - pressedTime > BTN_DEBOUNCE_TICKS){		// ��ǰʱ���밴��ʱ�̵Ĳ�ֵ�������󣬰����ȶ����ڰ��µ�ʱ��
			// ����������� ��δִ�лص�����
			if (onPressedCallback != NULL){
				onPressedCallback();	//ִ�лص�����
			}
			callbackState = 1;			//��־�ѵ��ûص����� �ұ����ظ����ûص�����
		}
	}else {
		// �����ɿ�
		pressedTime = 0;
		callbackState = 0;
	}
}



