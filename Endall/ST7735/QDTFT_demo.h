#ifndef __QDTFT_DEMO_H
#define __QDTFT_DEMO_H 
#include "stdio.h"


void Redraw_Mainmenu(void);
void Num_Test(void);
void Font_Test(void);
void Color_Test(void);
void showimage(const unsigned char *p); 
void QDTFT_Test_Demo(void);

void DHT11ReadHumiturePrint(void);

extern double hum;//Êª¶È
extern double tem;//ÎÂ¶È
#endif
