#ifndef __UART_H_
#define __UART_H_


/*****************************************************
					UARTͨѶ����
* ��       ��: UART.c
* ��       ��: Allen
* ˵       ��: ��������
* �� д ʱ �䣺2014-05-28
* ��	   ����V1.0
* �� �� ʱ �䣺��
* ----------------------------------------------------
* ע����

******************************************************/

void  InitUart(void);
void  SendUart(uint8_t dat);           //send data to PC

void uart2send(uint8_t* buff, uint16_t len);

#endif

