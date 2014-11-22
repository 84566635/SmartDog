#ifndef _315MHZ_H
#define _315MHZ_H

//#include "STC11F60XE.h"
//#include <intrins.h>
//#include "UART.h"
//#include "YuYin.h"
#include "stm32f10x.h"

  //Rx_315 = P1^0;


void RF315Init(void);

extern uint8_t short_k;     //窄脉冲宽度
extern uint8_t da1527[2][3];  //解码过程中临时数组 
extern uint8_t rf_ok;
extern uint8_t decode_ok;

void RF_decode(void);
void RF315_Rec(void);
#endif


