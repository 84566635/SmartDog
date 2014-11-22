#include "stm32f10x.h"
#include "315MHz.h"
#include "delay.h"
#include "uart.h"


//uint8_t bdata recv_flg  = 0;//uint8_t right or fault
//uint8_t bdata lianji_flg = 0; //long or short(1,0)

#define 	Rx_315   PAin(8)

uint8_t  recv_flg  = 0;//uint8_t right or fault
uint8_t  lianji_flg = 0; //long or short(1,0)


uint8_t short_k;     //խ������
uint8_t da1527[2][3];  //�����������ʱ���� 
uint8_t rf_ok;
uint8_t  decode_ok;

#define RF315DATA_PIN  	GPIO_Pin_8
#define RF315DATA_PORT	 GPIOA



 /*********************************************************/
 //	fan 
 //
 /*********************************************************/
void RF315Init(void)
{
   
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Reset GPIO init structure parameters values */
  GPIO_InitStruct.GPIO_Pin  = RF315DATA_PIN;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_Init(RF315DATA_PORT, &GPIO_InitStruct);
  		
}

/*****************************************************
fan end 
******************************************************/



void delay(uint16_t ms)//8.5US   
{ 
//	uint8_t i,j;
//	for(i=ms;i>0;i--)
//	{
//		for(j=21;j>0;j--)
//		_nop_();
//	}

	delay_us(8*ms);
}

void RF_decode() 
{
	uint8_t ii=0,j=0,k=0,rep=0;
	uint8_t head_k=0;           //��������
	//uint8_t s;
	//���ݽ���
	short_k=0;
	while(Rx_315 && j<250)
	{
		delay_us(1);	
		short_k++;

	}
	while(!Rx_315) 
	{
		delay(1);
		head_k++;
		
	}
	if(((short_k*24)<head_k) && (head_k<(short_k*38)))
	{
		for(rep=0;rep<2;rep++)
		{
			for(ii=0;ii<3;ii++)//3�ֽ�
			{
				for(k=0;k<8;k++)
				{
					j=0;
					while(Rx_315 && j<245) 
					{
						delay(1);
						j++;
					}
					if(j>(short_k-short_k/2-short_k/3)&&j<(short_k*1.96))
					{
						da1527[rep][ii]&=~(1<<((7-k)));
					}	
					else if(j>(short_k*1.96)&&j<(short_k*5))da1527[rep][ii]|=(1<<(7-k)); 	        			        
	             else {return;}          //�����˳�	
					j=0;
					while(!Rx_315 && j<150){delay(2);j++;}      //�����͵�ƽ 
				}
			}
			j=0;while(Rx_315 && (j<200)){delay(1);j++;}            //�������һ��������
			head_k=0;while(!Rx_315) {delay(1);head_k++;} //�����һ��ǰ���źŵČ���  
			if((head_k<(short_k*26)) || (head_k>(short_k*38)))  {return;}
		}
		if((da1527[0][0]==da1527[1][0]) && (da1527[0][1]==da1527[1][1]) && (da1527[0][2]==da1527[1][2]))	//���ν��յ���������ͬ
		{
			/*uint8_t u,i,x;
			rf_ok=1;
			for(i=0;i<3;i++)  //�ж�2262��1527
			{
				for(u=0;u<4;u++) {if(((da1527[0][i]>>(u*2)) & 3)==2) {i=80;break;}}  //��10��Ϊ1527
				if(i==80) break;
			}
			if(i==80)  //1527
			{
				key_d=da1527[1][2] & 0x0f;         //�ֳ�1527�İ���ֵ
				da1527[0][2]=da1527[1][2]>>4; //�ֳ�1527�ĺ�4λ��ַ
				jmnx=1;         //Ϊ0��2262��1��1527
			}
			else      //2262
			{
				key_d=0;
				for(i=0;i<4;i++){if(((da1527[0][2]>>(i*2))&3)==3) key_d|=1<<i;}   //�����2262�İ�������                                  
				da1527[0][2]=00; //2262�޺�4λ��ַ,ȫΪ0
				jmnx=0;         //Ϊ0��2262��1��1527
			}*/
			decode_ok=1;
		}
	}
	//PT2272_deco_exit:
	//decode_ok=0;
}

extern uint8_t _315MHz_Flag;
extern uint8_t  _315MHz_TimeCount2; 
void RF315_Rec(void)
{

	unsigned char i,j;
	RF_decode();
	if(decode_ok) 
	{
		decode_ok = 0;
		uart2send("WF<<",4);
		for(i=0;i<2;i++)
		{
			for(j=0;j<3;j++)
		//	SendUart(da1527[i][j]);
			uart2send(&da1527[i][j], 1);
		}
		uart2send("<<",2);
		_315MHz_Flag = 0;
		_315MHz_TimeCount2 = 1;
	}
}





/*void d40us(uint8_t n) //40us delay
{
	uint8_t data i,j;
	for(j=n;j>0;j--) 
	{ 
		for(i=100;i>0;i--)
		_nop_();
	}
}
unsigned long decode(void)
{
	unsigned long DATA;
	unsigned char i,count;
	DATA = 0;
	for(count=0;count<25;count++)
	{
		i=0;
		while(Rx_315&(1))
		{
			d40us(1);
			i++;
			if (i>45) goto end;
		}
		DATA*=2;
		if(i<12) 								 //�ֱ�ߵ�
		{
			if (!((i<12)&&(i>5)))  goto end;//��ƽ�Ϸ�
			DATA+=0;
		}
		else
		{
			if (!((i>24)&&(i<32))) goto end;
		  DATA+=1;  
		}
		
		//while (!(Rx_315&(1)))            //��λ����
		//{
			//d40us(1);
			//i++;
			//if (i>40) goto end;      //��ʱ���� 
			//else break;
		//}
		//if (i<3) goto end;      //�Աȿ��
	}
	return DATA;
	end:         
  return 0;
}*/
/*unsigned long decode(void)
{
	unsigned long DATA;
	uint8_t i,count;
	DATA=0; 
	for (count=0;count<24;count++) //start
	{
		i=0;                        //��ȼ���
		while(Rx_315&(1))           //�ߵ�ƽ
		{
			_nop_();
			i++;
			if (i>192) goto end;
		}
		DATA*=2;                   //����
		if (i<64) 								 //�ֱ�ߵ�
		{
			if (!((56>i)&(i>24)))DATA+=0 ; //��ƽ�Ϸ�
			else
			  goto end;   
		}
		else
		{
			if (!((168>i)&(i>72))) DATA+=1;
			else   
			  goto end;
		}
		while (!(Rx_315&(1)))            //��λ����
		{
			_nop_();
			i++;
			if (i>224) goto end;      //��ʱ����
		}
		if (i<96) goto end;      //�Աȿ��
	} 
  return DATA;	            //�ɹ�����
	end:         
  return 0;
}
*/
/*
unsigned long decode(void) 
{
	unsigned long DATA;
	uint8_t i,count;
	for(i=80;i;i--) //4ms delay search the rec
	{
		if(!(Rx_315))  
			i=80;
		else d50us(1);
	}
	for(i=40;i;i--)	//2mS
	{ 
		if(!Rx_315) break;
		else d50us(1);
	}
	if(i) 
	{
		recv_flg =0; 
		goto PT2272_deco_exit; 
	}
	for (count=0;count<24;count++)
	{
		i=0;
		 while(Rx_315)      
		 {       
			 _nop_(); 
			 i++;  
			 if (i>192) goto PT2272_deco_exit; 
		 } 
		 DATA*=2;
		 if (i<64)                             
		 {    
			 if (!((56>i)&(i>24))) goto PT2272_deco_exit;                                    
			 DATA+=0;                            
		 }  
		 else
		 {     
			 if (!((168>i)&(i>72))) goto PT2272_deco_exit;                                    
			 DATA+=1;                            
		 }
		 while (!(Rx_315))
		 {
			 _nop_(); 
			 i++;  
			 if (i>224) goto PT2272_deco_exit;
		 }
		 if (i<96) goto PT2272_deco_exit;
	}		
	return  DATA;  
	PT2272_deco_exit:
	return 0;
}*/


