#ifndef INT_TASK_MAIN_H_
#define INT_TASK_MAIN_H_

#include "kk_rtc.h"
#include "oled.h"
#include "usart.h"
#include "kk_knob.h"

#include <stdio.h>
#include <string.h>


void MainTask(void);
void MainTaskInit(void);
void showTime(struct tm* time);


#endif /* INT_TASK_MAIN_H_ */
