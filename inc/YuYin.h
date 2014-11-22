#ifndef _YUYIN_H
#define _YUYIN_H 
#include "stm32f10x.h"

void T1(void) ;
void T0_C1 (void);

void YuYinInit(void);
void yuyin(void);
void U1Init(void);
void T0Init(void);
void Timer1Init(void);
void Rstinit(void);
void U1_send(uint8_t i);
void U1_in(void);
void U1_sendS(unsigned char s[], unsigned int m);


#define  WIFI_LED PBout(7) //wifi复位LED指示灯
#define  WAKEUP_LED PAout(4) //唤醒状态指示灯

#define LED_ON   PBout(7)=1
#define LED_OFF  PBout(7)=0

#endif


