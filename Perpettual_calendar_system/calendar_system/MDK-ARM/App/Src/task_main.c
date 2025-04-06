#include "task_main.h"

#define CURSOR_FLASH_INTERVAL  	500	// ���ù����˸��� ��λms

char weeks[7][10] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

typedef struct {
	uint8_t x1;
	uint8_t y1;
	uint8_t x2;
	uint8_t y2;
} CursorPosition;					// �������

CursorPosition cursorPositions[6] = {
	{24 + 0 * 8, 17, 24 + 4 * 8, 17},			// Year
	{24 + 5 * 8, 17, 24 + 7 * 8, 17},			// Month
	{24 + 8 * 8, 17, 24 + 10 * 8, 17},		// Day
	{24 + 0 * 12, 45, 16 + 4 * 8, 45},		// Hour
	{24 + 3 * 12, 45, 16 + 4 * 8, 45},		// Minute
	{24 + 6 * 12, 45, 16 + 4 * 8, 45},		// Second
};

typedef enum {
	CalendarState_Normal,		// ��ͨģʽ
	CalendarState_Setting		// ����ģʽ
} CalendarState;					// ö�����ͣ�����ϵͳ������ģʽ

CalendarState calendarState = CalendarState_Normal;	// ��ʼģʽΪ��ͨģʽ

typedef enum {
	Year = 0,
	Month,
	Day,
	Hour,
	Minute,
	Second,
} SettingState;						// ö�����ͣ�������ť���º�����״̬

SettingState settingState = Year;	//��ʼ����״̬Ϊ��

typedef enum {
	Leap_year, 
	Unleap_year
} Leap_Year_Logo;		// �ж��Ƿ�Ϊ����

Leap_Year_Logo leapyearlogo = Unleap_year;							// ��ʼ������Ϊ������

struct tm settingTime; 	// ����ģʽ��,��Ҫ��һ���������õ�ʱ��,����һֱ��ȡ��ǰʱ��,δ����Ϊtm* ����Ϊ�ڽ�settingTime���ý�RTCʱ,������warning,Ϊ�������,����Ϊtm

/**
	* ��ת�ص����� onKnobForward()
	*/
void onKnobForward(){
/* ����ÿ���µ�������ͬ��ʹ��mktime����ת��Ϊunixʱ�����mktime���Խ��ܷǷ������ڣ���Ȼ������localtime(keil��gmtime������)ת��Ϊtm�ṹ������ */	
	if (calendarState == CalendarState_Setting){
		
		/* ʹ��mktime����ת��Ϊunixʱ���(mktime���Խ��ܷǷ�������) */
		time_t timestamp = mktime(&settingTime);
		
		switch (settingState){
			case Year:
				settingTime.tm_year++;
			break;
			
			case Month:
				settingTime.tm_mon++;
				if (settingTime.tm_mon > 11){
					settingTime.tm_mon = 0;
					settingTime.tm_year++;
				}
			break;
				
			case Day:
				settingTime.tm_mday++;
			break;									// ��������������жϣ�����tm�ṹת��ʱ���� 
			
			case Hour:
				settingTime.tm_hour++;
				if (settingTime.tm_hour > 23){
					settingTime.tm_hour = 0;
					settingTime.tm_mday++;
				}
			break;
			
			case Minute:
				settingTime.tm_min++;
				if (settingTime.tm_min > 59){
					settingTime.tm_min = 0;
					settingTime.tm_hour++;
				}
			break;
			
			case Second:
				settingTime.tm_sec++;
				if (settingTime.tm_sec > 59){
					settingTime.tm_sec = 0;
					settingTime.tm_min++;
				}
			break;
		}
		/* ����ʱ����ɺ��ٴ�ת��Ϊunixʱ��� */
		timestamp = mktime(&settingTime);
		
		/* ʹ��localtime ת��ʱ��� tm �ṹ�Ի����ȷ�����ں�ʱ�� */
		settingTime = *localtime(&timestamp);
	}
}

/**
	* ��ת�ص����� onKnobBackward()
	*/
void onKnobBackward(){
	if (calendarState == CalendarState_Setting){
		
		/* �Ƚ���ǰʱ��ת��Ϊʱ��� */ 
		time_t timestamp = mktime(&settingTime);
		
		switch (settingState){
			case Year:
				settingTime.tm_year--;
				if (settingTime.tm_year < 70){		//ȷ����ݲ�����1970����Ϊunixʱ�����1970��ʼ
					settingTime.tm_year = 70;
				}
			break;
			
			case Month:
				settingTime.tm_mon--;
				if (settingTime.tm_mon < 0){
					settingTime.tm_mon = 11;
					settingTime.tm_year--;
				}
			break;
				
			case Day:
				settingTime.tm_mday--;
			break;										// ��������������жϣ�����tm�ṹת��ʱ���� 
			
			case Hour:
				settingTime.tm_hour--;
				if (settingTime.tm_hour < 0){
					settingTime.tm_hour = 23;
					settingTime.tm_mday--;
				}
			break;
			
			case Minute:
				settingTime.tm_min--;
				if (settingTime.tm_min < 0){
					settingTime.tm_min = 59;
					settingTime.tm_hour--;
				}
			break;
			
			case Second:
				settingTime.tm_sec--;
				if (settingTime.tm_sec < 0){
					settingTime.tm_sec = 59;
					settingTime.tm_min--;
				}
			break;
		}
		/* ����ʱ����ɺ��ٴ�ת��Ϊunixʱ��� */
		timestamp = mktime(&settingTime);
		
		/* ʹ��localtime ת��ʱ��� tm �ṹ�Ի����ȷ�����ں�ʱ�� */
		settingTime = *localtime(&timestamp);
	}
}

/**
	* ��ť���»ص����� onKnobPressed()
	* ʹ��if���жϷǷ���ǿ�����õķ�ʽ��������
	*/
void onKnobPressed(){
	if (calendarState == CalendarState_Normal){
		settingTime = *KK_RTC_GetTime();						// ÿ�ν�������ģʽʱ��ʱ�䶼���л�������ģʽ��ʱ�� 
		settingState = Year;											// ÿ�ν�������ģʽʱ, ���꿪ʼ
		calendarState = CalendarState_Setting;
	}else{
		if(settingState == Second){
			KK_RTC_SetTime(&settingTime);						// ���л�����ͨģʽǰ��������ʱ�����õ�RTC�� 
			calendarState = CalendarState_Normal;
		} else{
			settingState++;													// ����ö�����ͱ���Ϊ���֣��˴�++�����л�����һ��ģʽ
		}
	}
	
}
/**
	* Ϊ������������Դ���г�ʼ��
	*/
void MainTaskInit(void){
	HAL_Delay(20);
	OLED_Init();		//��OLED���г�ʼ��
	
	KK_RTC_Init();	//��RTC���г�ʼ�����ǵ�����MX_RTC_Init������
	
	Knob_Init();		//��ť��ʼ��
	
	/* ������ť�ص� */
	//onForwardCallback()  ָ�� onKnobForward()  ��onForwardCallback()  ������ʱ ִ��onKnobForward()
	Knob_SetForwardCallback(onKnobForward);
	
	//onBackwardCallback() ָ�� onKnobBackward() ��onBackwardCallback() ������ʱ ִ��onKnobBackward()
	Knob_SetBackwardCallback(onKnobBackward);
	
	//onPressedCallback() ָ�� onKnobPressed() ��onPressedCallback() ������ʱ ִ��onKnobPressed()
	Knob_SetPressedCallback(onKnobPressed);
}

/**
	* ������ͨģʽ������ģʽ����Ҫ��ʾʱ�䣬�ʰ�װ��һ������
	*/
void showTime(struct tm* time){
	char str[30];
	
	/* ��������� */
	sprintf(str,"%d-%d-%d", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday);
	OLED_PrintASCIIString(24, 0, str, &afont16x8, OLED_COLOR_NORMAL);
	
	/* ���ʱ���� */
	sprintf(str,"%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec);
	OLED_PrintASCIIString(16, 20, str, &afont24x12, OLED_COLOR_NORMAL);
	
	/* ������ڼ� */
	char* week = weeks[time->tm_wday];											//now->tm_wday,��¼���ǵ�ǰ�ܵĵڼ��죬��0��6���������յ�����
	uint8_t x_weeks = (128 - (strlen(week) * 8)) / 2;			//128����Ļ��ȣ�8��ÿ���ַ��Ŀ�ȣ�������������������ַ���������
	OLED_PrintASCIIString(x_weeks, 48, week, &afont24x12, OLED_COLOR_NORMAL);
}

/**
	* �����ʾ����
	* startTime�뵱ǰʱ��Ĳ�ֵ С�ڹ����˸������޹����˸
	* startTime�뵱ǰʱ��Ĳ�ֵ ���ڹ����˸����������˸
	* ��startTime�뵱ǰʱ��Ĳ�ֵ ���ڹ����˸�������ʱ����startTime����Ϊ��ǰʱ��
	*/
void showCursor(){
	static uint32_t startTime = 0;		//����startTimeΪ��������ʼ��ʱ��������Ϊ��ǰʱ�䣬��ˣ�һ��ʼ�ʹ�����һ���жϣ���������Ϊ��ǰʱ��
	uint32_t difftime = HAL_GetTick() - startTime;
	if (difftime > 2 * CURSOR_FLASH_INTERVAL){
		startTime = HAL_GetTick();
	}else if (difftime > CURSOR_FLASH_INTERVAL){
		CursorPosition position = cursorPositions[settingState];		// ��̬��ȡ�������
		OLED_DrawLine(position.x1, position.y1, position.x2, position.y2, OLED_COLOR_NORMAL);
	}
}

/**
	* ��Ҫ�����ִ��
	*/
void MainTask(void){
	Knob_Loop();
	OLED_NewFrame();
	
	if (calendarState == CalendarState_Normal){
		struct tm* now = KK_RTC_GetTime();
		showTime(now);
	}else{
		showTime(&settingTime);
		showCursor();
	}
	
	
	OLED_ShowFrame();
}
