 #include "stm32f10x.h"
 #include "uart.h"

void InitUart()	
 {

	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStructure;
	

	//uart 时钟使能	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	//复用时钟打开
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_AFIO, ENABLE);

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    //gpio管脚时钟

	//uart2 串口参数
   	USART_InitStructure.USART_BaudRate = 115200;
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;
 	USART_InitStructure.USART_Parity = USART_Parity_No;
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
 	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		   //接收发送使能



     /*TX->PA2  USART_Mode_Tx  */
 	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_2;
  	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

		/*RX->PA3  USART_Mode_Rx  */
 	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_3;
  //	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

//	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);

	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);

 }


void SendUart(uint8_t dat)
{
	USART_SendData(USART2, (uint16_t) (dat));
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		
}
 
void uart2send(uint8_t* buff, uint16_t len)
{
	uint8_t* p;

	p = buff;

    while(len--)
	{
    /* Send one byte from USARTy to USARTz */
     USART_SendData(USART2,  (*p));
	    /* Loop until USARTy DR register is empty */ 
     while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	 p++;
	}	
}




