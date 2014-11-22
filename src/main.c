/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
//#include "stm32_eval.h"
#include <stdio.h>

#include "delay.h"
#include "stdio.h"
#include "spi.h"
#include "uart.h"
#include "24G_fun.h"
//#include "time.h"
#include "315MHZ.h"
#include "YuYin.h"
#include "DS18B20.h"


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
 USART_InitTypeDef USART_InitStructure;

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


#define PC        			0x01
#define PHONE     			0x02  //这几个都要交到串口进行处理
#define STM8_SWITCH			0x03	//
#define STM8_WIFI			0x04	//



uint8_t RecData[32];
uint8_t _315MHz_Flag;
unsigned int _315MHz_TimeCount;
uint8_t _315MHz_TimeCount2;
extern unsigned char UartRecFlag;
extern unsigned int j;
//unsigned int ;
int ted;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

int main(void)
{
  
	_315MHz_Flag = 0;
	_315MHz_TimeCount = 0;
	_315MHz_TimeCount2 = 1;
	//uint8_t test;
	


	YuYinInit();
//	WIFI_LED =LED_ON;// LED_ON;
//	WAKEUP_LED = LED_OFF;
//
//
	delay_init();
	InitUart();

//	RF315Init();//315Mhz.c


	T0Init();
    Timer1Init();                  
//	//	//Rstinit();
//	Init_DS18B20();	
//	init_nrf24l01_io();
//	ifnnrf_rx_mode();
//	delay_ms(50);
//	//while(NRF24L01_Check()){}  //检测nrf24l01	
//  
//	//TR1 = 0;
//   printf("love\r\n", 7);	
   while (1)
   {

  #if 0
	  if(_315MHz_Flag)
		{
			TIM_Cmd(TIM2, DISABLE);	// 关闭定时器时钟
			while(_315MHz_Flag)
			{
				//SendUart(0x55);
				RF315_Rec();//315接收代码
				_315MHz_TimeCount++;
				if(_315MHz_TimeCount == 500)
				{
					_315MHz_TimeCount = 0;
					_315MHz_TimeCount2++;
					if(_315MHz_TimeCount2++ >= 5)
					{
						_315MHz_TimeCount2 = 1;
					  _315MHz_Flag = 0;
					}
				}
			}	
				TIM_Cmd(TIM2, ENABLE);//开启定时器时钟
		}
		//RF315_Rec();//315接收代码   
		
		    if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
			{
				//SendUart(0x55);
				U1_in();//获取串口发送的数据!	
			}
			yuyin();
	#endif
  }
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART2, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
