#ifndef __24G_FUN_
#define __24G_FUN_

#define IRQ_OUT  PBout(8)	   //¿ªÂ©Êä³ö
#define IRQ_IN   PBin(8)

void RF24G_Init(void);
///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void Ifnnrf_SendData_Init(void);
uint8_t Ifnnrf_Send(uint8_t *txbuf);
uint8_t nRF24L01_RxPacket(uint8_t *rx_buf);
uint8_t NRF24L01_Check(void);
void do_2_4G(void);


#endif

