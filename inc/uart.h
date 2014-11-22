#ifndef __UART_H_
#define __UART_H_


/*****************************************************
					UART通讯驱动
* 文       件: UART.c
* 作       者: Allen
* 说       明: 串口驱动
* 编 写 时 间：2014-05-28
* 版	   本：V1.0
* 修 改 时 间：无
* ----------------------------------------------------
* 注：无

******************************************************/

void  InitUart(void);
void  SendUart(uint8_t dat);           //send data to PC

void uart2send(uint8_t* buff, uint16_t len);

#endif

