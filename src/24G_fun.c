#include "stm32f10x.h"
#include "spi.h"
#include "24G_fun.h"
#include "delay.h"
#include "uart.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
extern  unsigned char US[800];
/*****************************************************
					2.4G-功能
* 文       件: 24G_FUN.c
* 作       者: Allen				   
* 说       明: 2.4G-发送，接收，配置
* 编 写 时 间：2014-06-06
* 版	   本：V1.0
* 修 改 时 间：无
* ----------------------------------------------------
* 注：无

******************************************************/

void GpioIRQ_OUT_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	  /* IRQ PB8 ---------------------------------*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void GpioIRQ_IN_Init(void)
{

	GPIO_InitTypeDef   GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	  /* IRQ PB8 ---------------------------------*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void Ifnnrf_SendData_Init(void)
{
	int i = 0;
	//while(buf[i]!='\0')
	//tx_buf[0] = 0x7E;tx_buf[1] = 0x7E;
	for(i = 0;i < 7; i++)
		tx_buf[i] = i;
	tx_buf[7]=0x01;
	tx_buf[8]=0x01;
	for(i = 9;i < 32; i++)
	  tx_buf[i] = i;
}

uint8_t Ifnnrf_Send(uint8_t *txbuf)
{		
	uint8_t sta;

	GpioIRQ_OUT_Init();
	IRQ_OUT=1;

	SPI_RW_Reg(WRITE_REG1+STATUS,0xff);
	ifnnrf_tx_mode(txbuf);

	GpioIRQ_IN_Init();
	while(IRQ_IN);

	sta=SPI_Read(STATUS);
	
	SendUart(sta);

	SPI_RW_Reg(WRITE_REG1+STATUS,0xff);
	return sta;
}

uint8_t Ifnnrf_Receive()
{
	uint8_t sta,i;
	ifnnrf_rx_mode();

	GpioIRQ_OUT_Init();
	IRQ_OUT=1;

	GpioIRQ_IN_Init();
	while(IRQ_IN==0);

	sta=SPI_Read(STATUS);
	SendUart(sta);
	SPI_RW_Reg(WRITE_REG1+STATUS,0xff);
 	if(sta&STA_MARK_RX)
	{
		SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer;
		for(i=0;i<TX_PLOAD_WIDTH;i++)
			SendUart(rx_buf[i]);
	}
	SPI_RW_Reg(WRITE_REG1+STATUS,0xff);
	return sta;
}

uint8_t nRF24L01_RxPacket(uint8_t *rx_buf)
{
  uint8_t revale=0;
	uint8_t stata;
	//ifnnrf_rx_mode();
	//ifnnrf_rx_mode();
	//delay_ms(50);
	stata=SPI_Read(STATUS);	// read register STATUS's value
	SPI_RW_Reg(WRITE_REG1+STATUS,0xff);
	if(stata&0x40)				// if receive data ready (RX_DR) interrupt
	{   
			SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer		
			SPI_RW_Reg(FLUSH_RX,0xff);//??RX FIFO??? 
			stata |= 0x40;
			revale =1;//we have receive data    
	}

	return revale;
	
}

uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5]={0xa5,0xa5,0xa5,0xa5,0xa5};
	uint8_t buf1[5]={0xff,0xff,0xff,0xff,0xff};
	uint8_t i;  	 
	SPI_Write_Buf(WRITE_REG1+TX_ADDR,buf,5);//??5??????.	
	SPI_Read_Buf(TX_ADDR,buf1,5);              //???????  	
	for(i=0;i<5;i++)
		if(buf1[i]!=0xa5)
			break;					   
	if(i!=5)
		return 1;                               //NRF24L01???	
	return 0;		                                //NRF24L01??
}

extern unsigned char ted;
void do_2_4G(void)
{
	//Ifnnrf_SendData_Init(); //发送数据初始化
	Ifnnrf_Send(tx_buf);
	ifnnrf_rx_mode();
	delay_ms(50);
//			while(nRF24L01_RxPacket(rx_buf)!=1)
//			{
//				for(ted=0;ted<32;ted++)
//				{
//					SendUart(rx_buf[ted]);
//					break;
//				}
//				
//			}
}


  /////////////////////////////////////////////////////////////////////////////////






