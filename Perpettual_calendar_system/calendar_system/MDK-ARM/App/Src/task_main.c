#include "task_main.h"

#define CURSOR_FLASH_INTERVAL  	500	// 设置光标闪烁间隔 单位ms

char weeks[7][10] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

typedef struct {
	uint8_t x1;
	uint8_t y1;
	uint8_t x2;
	uint8_t y2;
} CursorPosition;					// 光标坐标

CursorPosition cursorPositions[6] = {
	{24 + 0 * 8, 17, 24 + 4 * 8, 17},			// Year
	{24 + 5 * 8, 17, 24 + 7 * 8, 17},			// Month
	{24 + 8 * 8, 17, 24 + 10 * 8, 17},		// Day
	{24 + 0 * 12, 45, 16 + 4 * 8, 45},		// Hour
	{24 + 3 * 12, 45, 16 + 4 * 8, 45},		// Minute
	{24 + 6 * 12, 45, 16 + 4 * 8, 45},		// Second
};

typedef enum {
	CalendarState_Normal,		// 普通模式
	CalendarState_Setting		// 设置模式
} CalendarState;					// 枚举类型，定义系统的两种模式

CalendarState calendarState = CalendarState_Normal;	// 初始模式为普通模式

typedef enum {
	Year = 0,
	Month,
	Day,
	Hour,
	Minute,
	Second,
} SettingState;						// 枚举类型，定义旋钮按下后，设置状态

SettingState settingState = Year;	//初始设置状态为年

typedef enum {
	Leap_year, 
	Unleap_year
} Leap_Year_Logo;		// 判断是否为闰年

Leap_Year_Logo leapyearlogo = Unleap_year;							// 初始化设置为非闰年

struct tm settingTime; 	// 设置模式下,需要有一个用于设置的时间,而非一直获取当前时间,未设置为tm* 则因为在将settingTime设置进RTC时,会引起warning,为避免出错,设置为tm

/**
	* 正转回调函数 onKnobForward()
	*/
void onKnobForward(){
/* 考虑每个月的天数不同。使用mktime将其转化为unix时间戳（mktime可以接受非法的日期），然后再用localtime(keil中gmtime有问题)转化为tm结构就行了 */	
	if (calendarState == CalendarState_Setting){
		
		/* 使用mktime将其转化为unix时间戳(mktime可以接受非法的日期) */
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
			break;									// 对于天数的溢出判断，将在tm结构转化时进行 
			
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
		/* 设置时间完成后，再次转化为unix时间戳 */
		timestamp = mktime(&settingTime);
		
		/* 使用localtime 转化时间戳 tm 结构以获得正确的日期和时间 */
		settingTime = *localtime(&timestamp);
	}
}

/**
	* 反转回调函数 onKnobBackward()
	*/
void onKnobBackward(){
	if (calendarState == CalendarState_Setting){
		
		/* 先将当前时间转化为时间戳 */ 
		time_t timestamp = mktime(&settingTime);
		
		switch (settingState){
			case Year:
				settingTime.tm_year--;
				if (settingTime.tm_year < 70){		//确保年份不低于1970，因为unix时间戳从1970开始
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
			break;										// 对于天数的溢出判断，将在tm结构转化时进行 
			
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
		/* 设置时间完成后，再次转化为unix时间戳 */
		timestamp = mktime(&settingTime);
		
		/* 使用localtime 转化时间戳 tm 结构以获得正确的日期和时间 */
		settingTime = *localtime(&timestamp);
	}
}

/**
	* 旋钮按下回调函数 onKnobPressed()
	* 使用if来判断非法并强制设置的方式并不优雅
	*/
void onKnobPressed(){
	if (calendarState == CalendarState_Normal){
		settingTime = *KK_RTC_GetTime();						// 每次进入设置模式时，时间都是切换进设置模式的时间 
		settingState = Year;											// 每次进入设置模式时, 从年开始
		calendarState = CalendarState_Setting;
	}else{
		if(settingState == Second){
			KK_RTC_SetTime(&settingTime);						// 在切换到普通模式前，将设置时间设置到RTC中 
			calendarState = CalendarState_Normal;
		} else{
			settingState++;													// 由于枚举类型本质为数字，此处++，即切换到下一个模式
		}
	}
	
}
/**
	* 为主任务的相关资源进行初始化
	*/
void MainTaskInit(void){
	HAL_Delay(20);
	OLED_Init();		//对OLED进行初始化
	
	KK_RTC_Init();	//对RTC进行初始化（记得设置MX_RTC_Init函数）
	
	Knob_Init();		//旋钮初始化
	
	/* 设置旋钮回调 */
	//onForwardCallback()  指向 onKnobForward()  当onForwardCallback()  被调用时 执行onKnobForward()
	Knob_SetForwardCallback(onKnobForward);
	
	//onBackwardCallback() 指向 onKnobBackward() 当onBackwardCallback() 被调用时 执行onKnobBackward()
	Knob_SetBackwardCallback(onKnobBackward);
	
	//onPressedCallback() 指向 onKnobPressed() 当onPressedCallback() 被调用时 执行onKnobPressed()
	Knob_SetPressedCallback(onKnobPressed);
}

/**
	* 由于普通模式和设置模式都需要显示时间，故包装成一个函数
	*/
void showTime(struct tm* time){
	char str[30];
	
	/* 输出年月日 */
	sprintf(str,"%d-%d-%d", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday);
	OLED_PrintASCIIString(24, 0, str, &afont16x8, OLED_COLOR_NORMAL);
	
	/* 输出时分秒 */
	sprintf(str,"%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec);
	OLED_PrintASCIIString(16, 20, str, &afont24x12, OLED_COLOR_NORMAL);
	
	/* 输出星期几 */
	char* week = weeks[time->tm_wday];											//now->tm_wday,记录的是当前周的第几天，从0到6，代表周日到周六
	uint8_t x_weeks = (128 - (strlen(week) * 8)) / 2;			//128是屏幕宽度，8是每个字符的宽度，二者相减除二，即该字符串的坐标
	OLED_PrintASCIIString(x_weeks, 48, week, &afont24x12, OLED_COLOR_NORMAL);
}

/**
	* 光标显示函数
	* startTime与当前时间的差值 小于光标闪烁间隔，无光标闪烁
	* startTime与当前时间的差值 大于光标闪烁间隔，光标闪烁
	* 当startTime与当前时间的差值 大于光标闪烁间隔两倍时，将startTime设置为当前时间
	*/
void showCursor(){
	static uint32_t startTime = 0;		//由于startTime为常量，初始化时不能设置为当前时间，因此，一开始就触发第一个判断，将其设置为当前时间
	uint32_t difftime = HAL_GetTick() - startTime;
	if (difftime > 2 * CURSOR_FLASH_INTERVAL){
		startTime = HAL_GetTick();
	}else if (difftime > CURSOR_FLASH_INTERVAL){
		CursorPosition position = cursorPositions[settingState];		// 动态获取光标坐标
		OLED_DrawLine(position.x1, position.y1, position.x2, position.y2, OLED_COLOR_NORMAL);
	}
}

/**
	* 主要任务的执行
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
