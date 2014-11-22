#include "spi.h"
#include "stm32f10x.h"
#include "delay.h"
#include "24G_fun.h"

#define  SPI2_PIN_SCK     GPIO_Pin_13
#define  SPI2_PIN_MOSI     GPIO_Pin_15
#define  SPI2_PIN_MISO     GPIO_Pin_14
#define  SPI2_PIN_NSS     GPIO_Pin_1
#define   SPI2_GPIO		   GPIOB

void SpiMsterGpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//gpio clck enable
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	//spi2 enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	

	  /* Configure SPIy pins: SCK, MISO and MOSI ---------------------------------*/
   GPIO_InitStructure.GPIO_Pin = SPI2_PIN_SCK | SPI2_PIN_MOSI;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_Init(SPI2_GPIO, &GPIO_InitStructure);

   GPIO_InitStructure.GPIO_Pin = SPI2_PIN_MISO;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(SPI2_GPIO, &GPIO_InitStructure);
   //nss
   GPIO_InitStructure.GPIO_Pin = SPI2_PIN_NSS;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init(SPI2_GPIO, &GPIO_InitStructure);

}

void SpiInit(void)
{
	SPI_InitTypeDef	  SPI_InitStructure;


	SpiMsterGpioInit();

	  /* SPIy Config -------------------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

  SPI_Cmd(SPI2, ENABLE);
}

//////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
#define uchar unsigned char


uint8_t  TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x43,0x10,0x10,0x01}; // Define a static TX address

uint8_t rx_buf[TX_PLOAD_WIDTH] = {0x01,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
                                0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};

uchar tx_buf[TX_PLOAD_WIDTH];

uchar flag;



/**************************************************/



//sbit LED1= P3^7;
/**************************************************/
unsigned char sta;

/**************************************************/

/**************************************************
Function: init_io();
Description:
  flash led one time,chip enable(ready to TX or RX Mode),
  Spi disable,Spi clock line init high
**************************************************/
#define KEY 0xaa
void init_nrf24l01_io(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;


  //clck enable
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  /*CE  output*/
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStruct);	 

  /*IRQ  output  输出高电平时外面接上拉电阻*/
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStruct);

  SpiInit();

	CE=0;			// chip enable
	CSN=1;			// Spi disable	
}

/**************************************************/

/**************************************************
Function: SPI_RW();

Description:
  Writes one uint8_t to nRF24L01, and return the uint8_t read
  from nRF24L01 during write, according to SPI protocol
**************************************************/
/*
uchar SPI_RW(uchar uint8_t)
{
	uchar bit_ctr;
   	for(bit_ctr=0;bit_ctr<8;bit_ctr++)   // output 8-bit
   	{
   		MOSI = (uint8_t & 0x80);         // output 'uint8_t', MSB to MOSI
   		uint8_t = (uint8_t << 1);           // shift next bit into MSB..
   		SCK = 1;                      // Set SCK high..
		MISO=1;
   		uint8_t |= MISO;       		  // capture current MISO bit
   		SCK = 0;            		  // ..then set SCK low again
   	}
    return(uint8_t);           		  // return read uint8_t
}
*/

uchar SPI_RW(uchar value)
{
	uint8_t st1;

	SPI_I2S_SendData(SPI2, value);

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
	 ;

	st1 = (uint8_t) SPI_I2S_ReceiveData(SPI2);
		
    return(st1);           		  // return read uint8_t
}
/**************************************************/

/**************************************************
Function: SPI_RW_Reg();

Description:
  Writes value 'value' to register 'reg'
*************************************************/
uchar SPI_RW_Reg(uint8_t reg, uint8_t value)
{
	uchar status;
  	CSN = 0;                   // CSN low, init SPI transaction
  	SPI_I2S_SendData(SPI2, reg);//SPI_RW(reg);      // select register
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	status = SPI_I2S_ReceiveData(SPI2);

  	SPI_I2S_SendData(SPI2, value);//SPI_RW(value);             // ..and write value to it..
  	CSN = 1;                   // CSN high again

//	SendUart(status);

  	return(status);            // return nRF24L01 status uint8_t
}
/**************************************************/

/**************************************************
Function: SPI_Read();

Description:
  Read one uint8_t from nRF24L01 register, 'reg'
*************************************************/
uint8_t SPI_Read(uint8_t reg)
{
	uint8_t reg_val;

  	CSN = 0;                // CSN low, initialize SPI communication...
  	SPI_I2S_SendData(SPI2, reg);//SPI_RW(reg);            // Select register to read from..

//  	reg_val = SPI_RW(0);    // ..then read registervalue
	SPI_I2S_SendData(SPI2, 0);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	reg_val = SPI_I2S_ReceiveData(SPI2);

  	CSN = 1;                // CSN high, terminate SPI communication

  	return(reg_val);        // return register value
}
/**************************************************/

/**************************************************
Function: SPI_Read_Buf();

Description:
  Reads 'uint8_ts' #of uint8_ts from register 'reg'
  Typically used to read RX payload, Rx/Tx address
**************************************************/
uchar SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uint8_ts)
{
	uchar status,uint8_t_ctr;

  	CSN = 0;                    		// Set CSN low, init SPI tranaction
  	status = SPI_RW(reg);       		// Select register to write to and read status uint8_t

  	for(uint8_t_ctr=0;uint8_t_ctr<uint8_ts;uint8_t_ctr++)
    	pBuf[uint8_t_ctr] = SPI_RW(0);    // Perform SPI_RW to read uint8_t from nRF24L01

  	CSN = 1;                           // Set CSN high again

//	SendUart(status);
  	return(status);                    // return nRF24L01 status uint8_t
}
/**************************************************/

/**************************************************
Function: SPI_Write_Buf();

Description:
  Writes contents of buffer '*pBuf' to nRF24L01
  Typically used to write TX payload, Rx/Tx address
**************************************************/
uchar SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uint8_ts)
{
	uchar status,uint8_t_ctr;

  	CSN = 0;                   // Set CSN low, init SPI tranaction
  	status = SPI_RW(reg);    // Select register to write to and read status uint8_t
  	for(uint8_t_ctr=0; uint8_t_ctr<uint8_ts; uint8_t_ctr++) // then write all uint8_t in buffer(*pBuf)
    	SPI_RW(*pBuf++);
  	CSN = 1;
//	SendUart(status);
	                 // Set CSN high again
  	return(status);          // return nRF24L01 status uint8_t
}
/**************************************************/

/**************************************************
Function: RX_Mode();

Description:
  This function initializes one nRF24L01 device to
  RX Mode, set RX address, writes RX payload width,
  select RF channel, datarate & LNA HCURR.
  After init, CE is toggled high, which means that
  this device is now ready to receive a datapacket.
**************************************************/
void power_off()
{
  				CE=0;
				SPI_RW_Reg(WRITE_REG1 + CONFIG, 0x0D); 
				CE=1;
				delay_us(20);
}
void ifnnrf_rx_mode(void)
{
    power_off();
	CE=0;
  	SPI_Write_Buf(WRITE_REG1 + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // Use the same address on the RX device as the TX device

  	SPI_RW_Reg(WRITE_REG1 + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
  	SPI_RW_Reg(WRITE_REG1 + EN_RXADDR, 0x01);  // Enable Pipe0
  	SPI_RW_Reg(WRITE_REG1 + RF_CH, 40);        // Select RF channel 40
  	SPI_RW_Reg(WRITE_REG1 + RX_PW_P0, TX_PLOAD_WIDTH); // Select same RX payload width as TX Payload width
  	SPI_RW_Reg(WRITE_REG1 + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
  	SPI_RW_Reg(WRITE_REG1 + CONFIG, 0x0f);     // Set PWR_UP bit, enable CRC(2 uint8_ts) & Prim:RX. RX_DR enabled..

  	CE = 1; // Set CE pin high to enable RX device

  //  This device is now ready to receive one packet of 16 uint8_ts payload from a TX device sending to address
  //  '3443101001', with auto acknowledgment, retransmit count of 10, RF channel 40 and datarate = 2Mbps.

}
/**************************************************/

/**************************************************
Function: TX_Mode();

Description:
  This function initializes one nRF24L01 device to
  TX mode, set TX address, set RX address for auto.ack,
  fill TX payload, select RF channel, datarate & TX pwr.
  PWR_UP is set, CRC(2 uint8_ts) is enabled, & PRIM:TX.

  ToDo: One high pulse(>10us) on CE will now send this
  packet and expext an acknowledgment from the RX device.
**************************************************/
void ifnnrf_tx_mode(uint8_t *txbuf)
{
    power_off();
	CE=0;
	
  	SPI_Write_Buf(WRITE_REG1 + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // Writes TX_Address to nRF24L01
  	SPI_Write_Buf(WRITE_REG1 + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // RX_Addr0 same as TX_Adr for Auto.Ack
  	SPI_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH); // Writes data to TX payload

  	SPI_RW_Reg(WRITE_REG1 + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
  	SPI_RW_Reg(WRITE_REG1 + EN_RXADDR, 0x01);  // Enable Pipe0
  	SPI_RW_Reg(WRITE_REG1 + SETUP_RETR, 0x1a); // 500us + 86us, 10 retrans...
  	SPI_RW_Reg(WRITE_REG1 + RF_CH, 40);        // Select RF channel 40
  	SPI_RW_Reg(WRITE_REG1 + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
  	SPI_RW_Reg(WRITE_REG1 + CONFIG, 0x0e);     // Set PWR_UP bit, enable CRC(2 uint8_ts) & Prim:TX. MAX_RT & TX_DS enabled..
	CE=1;

}

void SPI_CLR_Reg(uint8_t R_T)
{
  	CSN = 0; 
	if(R_T==1)                  // CSN low, init SPI transaction
  	SPI_RW(FLUSH_TX);             // ..and write value to it..
	else
	SPI_RW(FLUSH_RX);             // ..and write value to it..
  	CSN = 1;                   // CSN high again
}

void ifnnrf_CLERN_ALL()
{
  SPI_CLR_Reg(0);
  SPI_CLR_Reg(1);
  SPI_RW_Reg(WRITE_REG1+STATUS,0xff);
  IRQ_OUT=1;
}

