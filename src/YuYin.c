
#include "ds18b20.h"
#include <string.h>
#include "YuYin.h"
#include "uart.h"
#include "24G_fun.h"
#include "stm32f10x.h"
#include "delay.h"
#include "spi.h"

#define JS  PAin(6)//红外接收端口!
#define Y  PAout(5) //P2 ^ 5 //红外发射端口 	 
#define  RST PAout(15) //P1 ^ 1 //wifi复位RST 


uint8_t F = 0;	  //是否打开38KH方波调制
uint8_t Wifi_Command_Mode = 0; //=1 wifi工作在命令模式 =0 工作在数据传输模式
uint8_t Check_wifi = 1;		//检测wifi工作模式
uint8_t Get_Wifi_MAC = 0; //检测wifi模块MAC地址标志，只在STA模式下检测
unsigned int Wifi_MAC_Count = 0;
unsigned int Wifi_AP_OPEN_MODE = 0; //wifi工作在AP的OPEN模式下，灯闪烁
unsigned int RST_count1 = 0; //计数
unsigned int RST_count2 = 0;
unsigned char Temperature = 0; //温度
unsigned char UartRecFlag = 0; //串口接收终端标志
unsigned int T = 0;	//计数


unsigned int i = 0;//计数用 
unsigned int j = 0;//计数用
unsigned int c = 0;//计数用

unsigned int ui = 0;//串口接收数据长度!
unsigned char tx_test[17] = {0x7E,0x7E,0x34,0x43,0x10,0x10,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
 unsigned char US[800];//xdata unsigned char US[256]; //定义串口接收数据变量!

extern uint8_t _315MHz_Flag; //315检测数据标志
extern uint8_t _315MHz_TimeCount2;
/*
void Delay10us()		//@22.1184MHz
{
	unsigned char i;

	_nop_();
	i = 52;
	while (--i);
}
*/


void YuYinInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//gpio clck enable
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	  /* 红外接受JS pa6 ---------------------------------*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

	///红外发送 PA5 Y
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   	///wifi复位信号 RST PA15
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

      	///唤醒只是灯PA4
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

         	///wifi复位指示灯
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
   
}


void U1_in()//串口1接收数据
{



	j = 0; //超时退出!
	ui = 0;
	while(j < 40000)//超时退出(大约1ms)!需要测试此值是否正确! 5000
	{
		if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
		{
			US[ui] = USART_ReceiveData(USART2);;
			//SendUart(US[ui]);
			if(US[ui] == '<' && US[ui - 1] == '<')
			{
				UartRecFlag = 1;
				break;
			}
			ui++;
			j = 0;			
		}
		else
			j++;
//			UartRecFlag = 0;
		//Delay10us();//延时时间需要测试此值是否正确!(此处要加延时,要不数据接收不正确!)
	}	
}

/*void U1_send(unsigned char i)//串口1发送单字节数据
{
	TI = 0;			//令接收中断标志位为0（软件清零）
	SBUF = i;	//接收数据 SBUF 为单片机的接收发送缓冲寄存器
	while(TI==0);
	TI = 0;			//令接收中断标志位为0（软件清零）
}*/

void U1_sendS(unsigned char s[], unsigned int m)//串口1发送字符串数据,U1_sendS函数必须加"<<"结束标志!
{
	unsigned int n = 0;
	for(n = 0;n < m;n++)
		SendUart(s[n]);
}



/* TIM2中断优先级配置 */
void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*中断周期为13us*/
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_TimeBaseStructure.TIM_Period=13;	//自动重装载寄存器周期的值(计数值) 
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler= (24 - 1);	//  1M    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);	// 清除溢出中断标志 
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);	// 开启时钟    
   // BSP_IntVectSet(BSP_INT_ID_TIM2, TIMER2_Handler);
 }


void T0Init(void)		//13微秒，用的time2
{
	 TIM2_NVIC_Configuration();
     TIM2_Configuration();
}



void T0_C1 (void)  //单片机的中断号1对应的中断:定时器中断0
{		 
	T++;
	if(F == 1)
	    Y = ~Y;
}

typedef union //char型数据转int型数据类 
{  
	unsigned short int ue; 
	unsigned char 	 u[2]; 
}U16U8;
U16U8  M;//两个8位转16位

/*void U1Init(void)		//115200bps@22.1184MHz
{
	PCON |= 0x80;		//使能波特率倍速位SMOD
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//独立波特率发生器时钟为Fosc,即1T
	BRT = 0xF4;		//设定独立波特率发生器重装值
	AUXR |= 0x01;		//串口1选择独立波特率发生器为波特率发生器
	AUXR |= 0x10;		//启动独立波特率发生器
}*/

//void Rstinit()
//{
//	//配置为仅输入
//	P1M1 |= (1<<4);
//	P1M0 &= ~(1<<4);
//}

/*--------------
---wifi mode----
--------------*/

/* TIM3中断优先级配置 */
void TIM3_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*中断周期为5ms*/
void TIM3_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
    TIM_TimeBaseStructure.TIM_Period = 5;	//自动重装载寄存器周期的值(计数值) 
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler= (24000 - 1);	//  1M    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式 
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);	// 清除溢出中断标志 
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM3, ENABLE);	// 开启时钟    
   // BSP_IntVectSet(BSP_INT_ID_TIM2, TIMER2_Handler);
 }






void Timer1Init(void)		//5毫秒 timer3
{
	TIM3_NVIC_Configuration();
	TIM3_Configuration();
}







extern unsigned char ted;
extern unsigned char RxBuf[];
void T1(void) 
{
	unsigned char tedd;
	unsigned char Rec;
	if(Wifi_AP_OPEN_MODE)
	{
		Wifi_AP_OPEN_MODE++;
		if(Wifi_AP_OPEN_MODE == 50)
		{
			WIFI_LED = !WIFI_LED;
			Wifi_AP_OPEN_MODE = 1;
		}
	}
	else
	{
		Wifi_MAC_Count++;
		ted++;
		if(Wifi_MAC_Count == 3*200) //3秒定时
		{
			Get_Wifi_MAC = 1;
			Wifi_MAC_Count = 0;
		}
		if(ted == 1*200)
		{
			ted = 0;
			Rec = nRF24L01_RxPacket(rx_buf);
			if(Rec==1)
			{
				Rec = 0;
				if((rx_buf[0] == 0x7E)&&(rx_buf[1] == 0x7E)&&(rx_buf[2] == 0x34))
				{
					SendUart(0x7E);
					SendUart(0x7E);
					for(tedd=0;tedd<32;tedd++)
					{
						SendUart(rx_buf[tedd]);
							//UartRecFlag = 0;
					}
					U1_sendS("<<",2);
				}
				Rec = 0;
			}

#ifdef RF43M
			Rec = RF4432_RxPacket(RxBuf);
			if(Rec == 1)  //接收数据
		  {
				Rec = 0;
				//WAKEUP_LED = ~WAKEUP_LED;
				if(RxBuf[0] != RxBuf[1])
				{
					SendUart(0x7E);
					SendUart(0x7F);
					for(i=0;i<RxBuf_Len;i++)	
					{
						SendUart(RxBuf[i]);//Uart_sendB(RxBuf[i]);
					}
					U1_sendS("<<",2);
		 			SetRX_Mode();
				}
		  }
			else
			{
				SetRX_Mode();
			}
#endif
		}
	}
}

int start_wifi_command()
{
	U1_sendS("+++",3);
	memset(US,0x00,sizeof(US));	
	U1_in();
	if(US[0] == 'a')
	{	
		memset(US,0x00,sizeof(US));
		//Delay50ms();
		SendUart('a');
		U1_in();			
		if(strstr(US,"+ok") != NULL)
		{
			Wifi_Command_Mode = 1;
			memset(US,0x00,sizeof(US));
			return 0; //切换成功
		}	
	}
	memset(US,0x00,sizeof(US));
	return 1;
}

int start_wifi_data()
{
	U1_sendS("AT+ENTM\r\n",9);
	U1_in();
	if(strstr(US,"+ok") != NULL)
	{		
		Wifi_Command_Mode = 0;
		memset(US,0x00,sizeof(US));
		return 0; //切换成功
	}
	memset(US,0x00,sizeof(US));
	return 1;	
}
void yuyin(void)
{
	//CH:<< 			红外采集命令		//CH:长度+数据<<	//采集后返回的数据
	//FH:长度+数据<<	红外发射命令
	//FW:长度+数据<<  	无线发射命令
	//FS:<<				心跳
	//网络传来的是uint8_t格式的数据
	//while(1)
  //{
		#if 0
		if(Check_wifi)
		{
			TR1 = 0;
			if(!Wifi_Command_Mode)
			{
				start_wifi_command();
			}
			if(Wifi_Command_Mode)
			{
				Delay10ms();
				U1_sendS("AT+TCPB\r\n",9);
				U1_in();
				if(strstr(US,"on") != NULL ) //on模式WIFI_LED长亮，off闪烁
				{
					Wifi_AP_OPEN_MODE = 0;
					WIFI_LED = 1;
					U1_sendS("AT+ON\r\n",7);
				}
				else
				{
					Wifi_AP_OPEN_MODE = 1;   //off模式wifi_led闪烁  Wifi_AP_OPEN_MODE现在是TCPB变量没换
					U1_sendS("AT+OFF\r\n",8);
				}	
				Check_wifi = 0;
			}
			TR1 = 1;
		}	
		#endif
		
		if(RST==0)
		{
			 TIM_Cmd(TIM3, DISABLE);	// 开启时钟 
			WIFI_LED = RST;
			while(RST == 0)
			{
				RST_count1++;
				if(RST_count1 == 65535)
				{
					RST_count1 = 0;
					RST_count2++;
				}
			}
			if(RST_count2 >= 5)
			{
				Wifi_Command_Mode = 0;
				Get_Wifi_MAC = 0;
				Check_wifi = 1;
				RST_count1 = 0;
				RST_count2 = 0;
			}
			TIM_Cmd(TIM3, ENABLE);	// 开启时钟 
		}
		
		if(UartRecFlag == 1)
		{
			
			if(US[2] == ':')//接收到正确的控制数据!
			{
				TIM_Cmd(TIM3, DISABLE);	// 开启时钟
				switch(US[0])
				{
					case 'F'://红外、无线数据发射!
						WIFI_LED = LED_OFF;
						if(US[1]=='H')//红外
						{							
							i = 4;//第3与4位是数据长度,从第4位是红外、无线控制数据
							M.u[0] = US[3];
							M.u[1] = US[4];
							j = M.ue;						
						    TIM_Cmd(TIM2, ENABLE);	// 开启时钟 
							while(i < j)//j是数据长度-1!
							{
								T = 0;
								F = 1;
								i++;
								if(US[i] == 0)//&&US[i+1]==0)
								{
									i += 2;
									M.u[0] = US[i];
									i++;	
									M.u[1] = US[i];
								}
								else
								{
									M.u[0] = 0;	
									M.u[1] = US[i];
								}
								while(T < M.ue);

								T = 0;
								F = 0;
								Y = 1;
								i++;
								if(US[i] == 0)//&&uip_appdata[i+1]==0)
								{
									i += 2;
									M.u[0] = US[i];
									i++;	
									M.u[1] = US[i];
								}
								else
								{
									M.u[0] = 0;	
									M.u[1] = US[i];
								}
								while(T < M.ue);								
							}
							TIM_Cmd(TIM2, DISABLE);	// 开启时钟 		//关闭定时器0
							U1_sendS("FH<<", 4); 
							WIFI_LED = LED_ON;
						}
						else if(US[1]=='W')
						{						
							c = 0;
							TIM_Cmd(TIM2, ENABLE);	// 开启时钟 
							while(c < 6)//重复次数!
							{
								T = 0;

								i = 4;//第3与4位是数据长度,从第5位是红外、无线控制数据
								while(T < 28);//(13 * 808 = 10504同步脉宽!									
								T = 0;
								M.u[0] = US[3];
								M.u[1] = US[4];
								j = M.ue;//主机生成的长度要减1
								while(T < 808);//(13 * 808 = 10504同步脉宽!

								while(i < j)
								{
									T = 0;
									i++;
									while(T < US[i]);

									T = 0;
									i++;//i在此,精准一些
									while(T < US[i]);
								}
								c++;
							}
							TIM_Cmd(TIM2, DISABLE);	// GUAN时钟 
							U1_sendS("FW<<", 4);
						}
						else if(US[1]=='S')
						{
							U1_sendS("FS<<", 4);
						}

						break;

					case 'C'://红外采集!
				   	U1_sendS("CA<<", 4);//返回到主机请按遥控器("<<"在U1_sendS中添加)
						i = 5;//第3与4位是数据长度,从第4位是红外、无线控制数据
						j = 0;
						TIM_Cmd(TIM2, ENABLE);	// GUAN时钟 		//启动定时器0
						while(i < 756) //长度给这句有关-->>US[2] = i;//第三位是数据长度
						{ 
							T = 1;   //应该能提高准确率
     	  			while(JS == 0);
   	  				if(T > 5)
							{
								M.ue = T;
	              T = 1;
								if(M.u[0] > 0)
								{
									US[i] = 0;	//将接收的数据发送回去（删除//即生效）
									i++;
									US[i] = 0;	//将接收的数据发送回去（删除//即生效）
									i++;
									US[i] = M.u[0];	//将接收的数据发送回去（删除//即生效）
									i++;
								}
								US[i] = M.u[1];
								i++;	
								while(JS == 1)
								{								
									if(T > 6000)//无数据退出								
									{
										US[i] = 0;
										i++;

										M.ue = i;
										US[3] = M.u[0];//第3与4位是数据长度(包括数据头,不包括结尾!)
										US[4] = M.u[1];//第3与4位是数据长度(包括数据头,不包括结尾!)
									 		   
										US[i] = '<';
										i++;
										US[i] = '<';
										i++;

										US[0] = 'C';
										US[1] = 'H';
										US[2] = ':';																	
										U1_sendS(US, i);//红外采集成功

										i = 756;
										break;
									}
								}
								if(i < 756)
								{				
									M.ue = T;
				
									if(M.u[0] > 0)
									{
										US[i] = 0;
										i++;
										US[i] = 0;
										i++;
										US[i] = M.u[0];
										i++;
									}
									US[i] = M.u[1];
									i++;
									j = 0;
								}
							}
							else
							{
								while(JS == 1)
								{
									if(T > 50000)
									{
										T = 0;
										j++;
										if(j > 30)
										{
											i = 756;
											U1_sendS("CC<<", 4);//超时退出!大约20秒无操作退出!
											break;
										}
									}
								}
							}
						}
						TIM_Cmd(TIM2, DISABLE);	// GUAN时钟 		//关闭定时器0
						break;
					case 'D':
					_315MHz_Flag = 1;
			#ifdef RF433M
			
						Si4432_Init();
							SetTX_Mode();		// 每间隔一段时间，发射一包数据，并接收 Acknowledge 信号
							RF4432_TxPacket(TxBuf);
							//CLOSE_RX_OK;	//熄灭指定的LED
							SetRX_Mode();
							delay_ms(200);
			#endif

							U1_sendS("BF<<",4);	
					
							switch(US[1])
							{
								case 'T'://温度
												memset(US,0x00,sizeof(US));
												US[0] = 'D';
												US[1] = 'T';
												while((US[2] = GetTemperature()) == 0x55);
												US[3] = '<';
												US[4] = '<';
												U1_sendS(US, 5);	
												break;
								case 'S'://检测wifi工作模式
												Check_wifi = 1;
												Wifi_Command_Mode = 0;
												U1_sendS("DS<<",4);
												break;
								default:break;				
							}
							break;
					case 'L': //唤醒状态指示灯
							if(US[1] == 'B')
							{
									int i = 0;
									for(i = 0;i < 17; i++)
										tx_buf[i] = tx_test[i];
									tx_buf[17]=0x01;
									tx_buf[18]=0x01;
									for(i = 19;i < 32; i++)
										tx_buf[i] = i;
								do_2_4G();
								U1_sendS("TF<<",4);
								WAKEUP_LED = LED_ON;
								U1_sendS("LB<<",4);
							}	
							else if(US[1] == 'D')
							{
									int i = 0;
									for(i = 0;i < 17; i++)
										tx_buf[i] = tx_test[i];
									tx_buf[17]=0x01;
									tx_buf[18]=0x00;
									for(i = 19;i < 32; i++)
										tx_buf[i] = i;
								do_2_4G();
								U1_sendS("TF<<",4);		
								WAKEUP_LED = LED_OFF;
								U1_sendS("LD<<",4);
							}
							break;
					//新加代码
					case 'G': //2.4 GH: 第3,4位是数据长度，从第5位是数据位
						//int i = 0;
						for(i=0;i<32;i++)
						{
							tx_buf[i] = US[i+3];
							//SendUart(tx_buf[i]);
						}
 						do_2_4G();
					  U1_sendS("TF<<",4);
					break;
					case 'B':  //433 BH: 第3,4位是数据长度，从第5位是数据位
#ifdef RF433M

							Si4432_Init();
							SetTX_Mode();		// 每间隔一段时间，发射一包数据，并接收 Acknowledge 信号
							RF4432_TxPacket(TxBuf);
							//CLOSE_RX_OK;	//熄灭指定的LED
							SetRX_Mode();

							delay_ms(200);
#endif
							U1_sendS("BF<<",4);
						break;
					case 'W':
						_315MHz_Flag = 1;
					_315MHz_TimeCount2 = 1;
						delay_ms(200);
						U1_sendS("WF<<",4);
					  break;
					default:break;	
				}
				TIM_Cmd(TIM3, ENABLE);	// 开启时钟
			}
			#if 0
			else if(strstr(US,"+o") != NULL) //收到wifi模块返回的数据 +ok
			{
				//if(strstr(US,"AP") != NULL) 	//wifi工作在AP模式
				if(strstr(US,"on") != NULL  || strstr(US,"off") != NULL)
				{
					Delay10ms();
					U1_sendS("AT+WAKEY\r\n",10);
				}
				else if(strstr(US,"OPEN") != NULL) //AP模式下的open加密  
				{
					Check_wifi = 0;
					Wifi_AP_OPEN_MODE = 1;
					TR1 = 1;
					if(!start_wifi_data())
					{
						Check_wifi = 0;
						Wifi_Command_Mode = 0;
					}
				}
				else
				{
					if(!start_wifi_data())
					{
						Check_wifi = 0;
						Wifi_Command_Mode = 0;
						Wifi_AP_OPEN_MODE = 0;
					}
				}
			}
			#endif
			UartRecFlag = 0;
		}
		US[2] = 0x00;//一个串口命令执行完毕, 清空
		
	//}
}


