#ifndef _TIMER_H
#define _TIMER_H


void    isr_13us(void);

void    timer2_init(void);

void    timer2_enable(void);

void    timer2_disable(void);

void    Enable_SysTick(void);

void    Disable_SysTick(void);



void Timer2_Handle(void);











///////////////////////////////////////
void TimeInit(void);

#endif


