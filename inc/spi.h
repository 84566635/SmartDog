#ifndef __SPI_H_  
#define __SPI_H_
#include "stm32f10x.h"

#define CSN  PBout(1)
#define CE   PBout(0)

void SpiInit(void);



////////////////////////////////////////////////
/////////////////////////////////////////////////
#define TX_ADR_WIDTH    5   // 5  uint8_ts TX(RX) address width

#define TX_PLOAD_WIDTH  32  // 32 uint8_ts TX payload


#define READ_REG1        0x00  // Define read command to register
#define WRITE_REG1       0x20  // Define write command to register
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define FLUSH_TX        0xE1  // Define flush TX register command
#define FLUSH_RX        0xE2  // Define flush RX register command
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command
#define NOP             0xFF  // Define No Operation, might be used to read status register

//***************************************************//
// SPI(nRF24L01) registers(addresses)
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address
#define STA_MARK_RX     0X40
#define STA_MARK_TX     0X20
#define STA_MARK_MX     0X10	    


//***************************************************************//
//                   FUNCTION's PROTOTYPES  //
/****************************************************************
 void SPI_Init(uint8_t Mode);     // Init HW or SW SPI
 uint8_t SPI_RW(uint8_t uint8_t);                                // Single SPI read/write
 uint8_t SPI_Read(uint8_t reg);                               // Read one uint8_t from nRF24L01
 uint8_t SPI_RW_Reg(uint8_t reg, uint8_t uint8_t);                  // Write one uint8_t to register 'reg'
 uint8_t SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uint8_ts);  // Writes multiply uint8_ts to one register
 uint8_t SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uint8_ts);   // Read multiply uint8_ts from one register
*****************************************************************/
  void NRF24L01_SPI_Init(uint8_t Mode);     // Init HW or SW SPI
  uint8_t SPI_RW(uint8_t uint8_t);                                // Single SPI read/write
  uint8_t SPI_Read(uint8_t reg);                               // Read one uint8_t from nRF24L01
  uint8_t SPI_RW_Reg(uint8_t reg, uint8_t uint8_t);                  // Write one uint8_t to register 'reg'
  uint8_t SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uint8_ts);  // Writes multiply uint8_ts to one register
  uint8_t SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uint8_ts);   // Read multiply uint8_ts from one register

  void init_nrf24l01_io(void);
  void ifnnrf_rx_mode(void);
  void ifnnrf_tx_mode(uint8_t *txbuf);
  void ifnnrf_CLERN_ALL(void);

  extern unsigned char tx_buf[];
  extern unsigned char rx_buf[];

  extern unsigned char	 sta;


#endif

