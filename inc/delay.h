#ifndef __DELAY_H
#define __DELAY_H 			   
//#include <stm32f10x_lib.h>
#include "stm32f10x.h"

////////////////////////////////////////////////////////////////////////////////// 
void delay_init(void);
void delay_s(u16 s);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif





























