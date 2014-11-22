
#include "ds18b20.h"
#include <string.h>
#include "YuYin.h"
#include "uart.h"
#include "24G_fun.h"
#include "stm32f10x.h"
#include "delay.h"
#include "spi.h"

#define JS  PAin(6)//������ն˿�!
#define Y  PAout(5) //P2 ^ 5 //���ⷢ��˿� 	 
#define  RST PAout(15) //P1 ^ 1 //wifi��λRST 


uint8_t F = 0;	  //�Ƿ��38KH��������
uint8_t Wifi_Command_Mode = 0; //=1 wifi����������ģʽ =0 ���������ݴ���ģʽ
uint8_t Check_wifi = 1;		//���wifi����ģʽ
uint8_t Get_Wifi_MAC = 0; //���wifiģ��MAC��ַ��־��ֻ��STAģʽ�¼��
unsigned int Wifi_MAC_Count = 0;
unsigned int Wifi_AP_OPEN_MODE = 0; //wifi������AP��OPENģʽ�£�����˸
unsigned int RST_count1 = 0; //����
unsigned int RST_count2 = 0;
unsigned char Temperature = 0; //�¶�
unsigned char UartRecFlag = 0; //���ڽ����ն˱�־
unsigned int T = 0;	//����


unsigned int i = 0;//������ 
unsigned int j = 0;//������
unsigned int c = 0;//������

unsigned int ui = 0;//���ڽ������ݳ���!
unsigned char tx_test[17] = {0x7E,0x7E,0x34,0x43,0x10,0x10,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
 unsigned char US[800];//xdata unsigned char US[256]; //���崮�ڽ������ݱ���!

extern uint8_t _315MHz_Flag; //315������ݱ�־
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

	  /* �������JS pa6 ---------------------------------*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

	///���ⷢ�� PA5 Y
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   	///wifi��λ�ź� RST PA15
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

      	///����ֻ�ǵ�PA4
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

         	///wifi��λָʾ��
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
   
}


void U1_in()//����1��������
{



	j = 0; //��ʱ�˳�!
	ui = 0;
	while(j < 40000)//��ʱ�˳�(��Լ1ms)!��Ҫ���Դ�ֵ�Ƿ���ȷ! 5000
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
		//Delay10us();//��ʱʱ����Ҫ���Դ�ֵ�Ƿ���ȷ!(�˴�Ҫ����ʱ,Ҫ�����ݽ��ղ���ȷ!)
	}	
}

/*void U1_send(unsigned char i)//����1���͵��ֽ�����
{
	TI = 0;			//������жϱ�־λΪ0��������㣩
	SBUF = i;	//�������� SBUF Ϊ��Ƭ���Ľ��շ��ͻ���Ĵ���
	while(TI==0);
	TI = 0;			//������жϱ�־λΪ0��������㣩
}*/

void U1_sendS(unsigned char s[], unsigned int m)//����1�����ַ�������,U1_sendS���������"<<"������־!
{
	unsigned int n = 0;
	for(n = 0;n < m;n++)
		SendUart(s[n]);
}



/* TIM2�ж����ȼ����� */
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

/*�ж�����Ϊ13us*/
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_TimeBaseStructure.TIM_Period=13;	//�Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) 
    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    TIM_TimeBaseStructure.TIM_Prescaler= (24 - 1);	//  1M    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);	// �������жϱ�־ 
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);	// ����ʱ��    
   // BSP_IntVectSet(BSP_INT_ID_TIM2, TIMER2_Handler);
 }


void T0Init(void)		//13΢�룬�õ�time2
{
	 TIM2_NVIC_Configuration();
     TIM2_Configuration();
}



void T0_C1 (void)  //��Ƭ�����жϺ�1��Ӧ���ж�:��ʱ���ж�0
{		 
	T++;
	if(F == 1)
	    Y = ~Y;
}

typedef union //char������תint�������� 
{  
	unsigned short int ue; 
	unsigned char 	 u[2]; 
}U16U8;
U16U8  M;//����8λת16λ

/*void U1Init(void)		//115200bps@22.1184MHz
{
	PCON |= 0x80;		//ʹ�ܲ����ʱ���λSMOD
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x04;		//���������ʷ�����ʱ��ΪFosc,��1T
	BRT = 0xF4;		//�趨���������ʷ�������װֵ
	AUXR |= 0x01;		//����1ѡ����������ʷ�����Ϊ�����ʷ�����
	AUXR |= 0x10;		//�������������ʷ�����
}*/

//void Rstinit()
//{
//	//����Ϊ������
//	P1M1 |= (1<<4);
//	P1M0 &= ~(1<<4);
//}

/*--------------
---wifi mode----
--------------*/

/* TIM3�ж����ȼ����� */
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

/*�ж�����Ϊ5ms*/
void TIM3_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
    TIM_TimeBaseStructure.TIM_Period = 5;	//�Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) 
    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    TIM_TimeBaseStructure.TIM_Prescaler= (24000 - 1);	//  1M    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ 
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);	// �������жϱ�־ 
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM3, ENABLE);	// ����ʱ��    
   // BSP_IntVectSet(BSP_INT_ID_TIM2, TIMER2_Handler);
 }






void Timer1Init(void)		//5���� timer3
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
		if(Wifi_MAC_Count == 3*200) //3�붨ʱ
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
			if(Rec == 1)  //��������
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
			return 0; //�л��ɹ�
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
		return 0; //�л��ɹ�
	}
	memset(US,0x00,sizeof(US));
	return 1;	
}
void yuyin(void)
{
	//CH:<< 			����ɼ�����		//CH:����+����<<	//�ɼ��󷵻ص�����
	//FH:����+����<<	���ⷢ������
	//FW:����+����<<  	���߷�������
	//FS:<<				����
	//���紫������uint8_t��ʽ������
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
				if(strstr(US,"on") != NULL ) //onģʽWIFI_LED������off��˸
				{
					Wifi_AP_OPEN_MODE = 0;
					WIFI_LED = 1;
					U1_sendS("AT+ON\r\n",7);
				}
				else
				{
					Wifi_AP_OPEN_MODE = 1;   //offģʽwifi_led��˸  Wifi_AP_OPEN_MODE������TCPB����û��
					U1_sendS("AT+OFF\r\n",8);
				}	
				Check_wifi = 0;
			}
			TR1 = 1;
		}	
		#endif
		
		if(RST==0)
		{
			 TIM_Cmd(TIM3, DISABLE);	// ����ʱ�� 
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
			TIM_Cmd(TIM3, ENABLE);	// ����ʱ�� 
		}
		
		if(UartRecFlag == 1)
		{
			
			if(US[2] == ':')//���յ���ȷ�Ŀ�������!
			{
				TIM_Cmd(TIM3, DISABLE);	// ����ʱ��
				switch(US[0])
				{
					case 'F'://���⡢�������ݷ���!
						WIFI_LED = LED_OFF;
						if(US[1]=='H')//����
						{							
							i = 4;//��3��4λ�����ݳ���,�ӵ�4λ�Ǻ��⡢���߿�������
							M.u[0] = US[3];
							M.u[1] = US[4];
							j = M.ue;						
						    TIM_Cmd(TIM2, ENABLE);	// ����ʱ�� 
							while(i < j)//j�����ݳ���-1!
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
							TIM_Cmd(TIM2, DISABLE);	// ����ʱ�� 		//�رն�ʱ��0
							U1_sendS("FH<<", 4); 
							WIFI_LED = LED_ON;
						}
						else if(US[1]=='W')
						{						
							c = 0;
							TIM_Cmd(TIM2, ENABLE);	// ����ʱ�� 
							while(c < 6)//�ظ�����!
							{
								T = 0;

								i = 4;//��3��4λ�����ݳ���,�ӵ�5λ�Ǻ��⡢���߿�������
								while(T < 28);//(13 * 808 = 10504ͬ������!									
								T = 0;
								M.u[0] = US[3];
								M.u[1] = US[4];
								j = M.ue;//�������ɵĳ���Ҫ��1
								while(T < 808);//(13 * 808 = 10504ͬ������!

								while(i < j)
								{
									T = 0;
									i++;
									while(T < US[i]);

									T = 0;
									i++;//i�ڴ�,��׼һЩ
									while(T < US[i]);
								}
								c++;
							}
							TIM_Cmd(TIM2, DISABLE);	// GUANʱ�� 
							U1_sendS("FW<<", 4);
						}
						else if(US[1]=='S')
						{
							U1_sendS("FS<<", 4);
						}

						break;

					case 'C'://����ɼ�!
				   	U1_sendS("CA<<", 4);//���ص������밴ң����("<<"��U1_sendS�����)
						i = 5;//��3��4λ�����ݳ���,�ӵ�4λ�Ǻ��⡢���߿�������
						j = 0;
						TIM_Cmd(TIM2, ENABLE);	// GUANʱ�� 		//������ʱ��0
						while(i < 756) //���ȸ�����й�-->>US[2] = i;//����λ�����ݳ���
						{ 
							T = 1;   //Ӧ�������׼ȷ��
     	  			while(JS == 0);
   	  				if(T > 5)
							{
								M.ue = T;
	              T = 1;
								if(M.u[0] > 0)
								{
									US[i] = 0;	//�����յ����ݷ��ͻ�ȥ��ɾ��//����Ч��
									i++;
									US[i] = 0;	//�����յ����ݷ��ͻ�ȥ��ɾ��//����Ч��
									i++;
									US[i] = M.u[0];	//�����յ����ݷ��ͻ�ȥ��ɾ��//����Ч��
									i++;
								}
								US[i] = M.u[1];
								i++;	
								while(JS == 1)
								{								
									if(T > 6000)//�������˳�								
									{
										US[i] = 0;
										i++;

										M.ue = i;
										US[3] = M.u[0];//��3��4λ�����ݳ���(��������ͷ,��������β!)
										US[4] = M.u[1];//��3��4λ�����ݳ���(��������ͷ,��������β!)
									 		   
										US[i] = '<';
										i++;
										US[i] = '<';
										i++;

										US[0] = 'C';
										US[1] = 'H';
										US[2] = ':';																	
										U1_sendS(US, i);//����ɼ��ɹ�

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
											U1_sendS("CC<<", 4);//��ʱ�˳�!��Լ20���޲����˳�!
											break;
										}
									}
								}
							}
						}
						TIM_Cmd(TIM2, DISABLE);	// GUANʱ�� 		//�رն�ʱ��0
						break;
					case 'D':
					_315MHz_Flag = 1;
			#ifdef RF433M
			
						Si4432_Init();
							SetTX_Mode();		// ÿ���һ��ʱ�䣬����һ�����ݣ������� Acknowledge �ź�
							RF4432_TxPacket(TxBuf);
							//CLOSE_RX_OK;	//Ϩ��ָ����LED
							SetRX_Mode();
							delay_ms(200);
			#endif

							U1_sendS("BF<<",4);	
					
							switch(US[1])
							{
								case 'T'://�¶�
												memset(US,0x00,sizeof(US));
												US[0] = 'D';
												US[1] = 'T';
												while((US[2] = GetTemperature()) == 0x55);
												US[3] = '<';
												US[4] = '<';
												U1_sendS(US, 5);	
												break;
								case 'S'://���wifi����ģʽ
												Check_wifi = 1;
												Wifi_Command_Mode = 0;
												U1_sendS("DS<<",4);
												break;
								default:break;				
							}
							break;
					case 'L': //����״ָ̬ʾ��
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
					//�¼Ӵ���
					case 'G': //2.4 GH: ��3,4λ�����ݳ��ȣ��ӵ�5λ������λ
						//int i = 0;
						for(i=0;i<32;i++)
						{
							tx_buf[i] = US[i+3];
							//SendUart(tx_buf[i]);
						}
 						do_2_4G();
					  U1_sendS("TF<<",4);
					break;
					case 'B':  //433 BH: ��3,4λ�����ݳ��ȣ��ӵ�5λ������λ
#ifdef RF433M

							Si4432_Init();
							SetTX_Mode();		// ÿ���һ��ʱ�䣬����һ�����ݣ������� Acknowledge �ź�
							RF4432_TxPacket(TxBuf);
							//CLOSE_RX_OK;	//Ϩ��ָ����LED
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
				TIM_Cmd(TIM3, ENABLE);	// ����ʱ��
			}
			#if 0
			else if(strstr(US,"+o") != NULL) //�յ�wifiģ�鷵�ص����� +ok
			{
				//if(strstr(US,"AP") != NULL) 	//wifi������APģʽ
				if(strstr(US,"on") != NULL  || strstr(US,"off") != NULL)
				{
					Delay10ms();
					U1_sendS("AT+WAKEY\r\n",10);
				}
				else if(strstr(US,"OPEN") != NULL) //APģʽ�µ�open����  
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
		US[2] = 0x00;//һ����������ִ�����, ���
		
	//}
}


