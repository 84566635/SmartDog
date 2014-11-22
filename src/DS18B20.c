#include "stm32f10x.h"
#include "ds18b20.h"
#include "delay.h"

#define  DQ 	  PAout(1)
#define  DQ_SAMPLE	  PAin(1)



extern void U1_send(unsigned char i);


void DS18B20_PINOUT_Conf(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	//gpio clck enable
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	  /* PA1 ---------------------------------*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
}



void Init_DS18B20()
{


    DS18B20_PINOUT_Conf();


	DQ = 1;
	delay_us(10);
	DQ = 0;
	delay_us(510);
	DQ = 1;
	delay_us(30);
	if(DQ_SAMPLE)
	{
		delay_us(30);
	}
	delay_us(510);
	DQ = 1;	
}

unsigned char ReadOneChar()
{
	unsigned char i,dat=0;
	for(i = 0;i < 8;i++)
	{
		dat >>= 1;	
		DQ = 1;
		delay_us(1);		
		DQ = 0;
delay_us(1);	
		DQ = 1;
delay_us(2);		
		if(DQ_SAMPLE)
			dat |= 0x80;
		delay_us(35);
	}
	DQ = 1;
	delay_us(35);
	return dat;
}

void WriteOneChar(unsigned char dat)
{
	unsigned char i;
	for(i = 0;i < 8;i++)
	{
		DQ = 0;
		delay_us(15);
		DQ = dat&0x01;
		dat >>= 1;
		delay_us(45);
		DQ = 1;
	}	
	DQ = 1;
	delay_us(65);
}


int GetTemperature(void)
{
	unsigned char a=0;
	unsigned char b=0;
	int Temperature;
	int tmp = 0;

	Init_DS18B20();
	WriteOneChar(0xCC); // 跳过读序号列号的操作
	WriteOneChar(0x44); // 启动温度转换
	delay_us(510);
	Init_DS18B20();
	WriteOneChar(0xCC); //跳过读序号列号的操作 
	WriteOneChar(0xBE); //读取温度寄存器等（共可读9个寄存器） 前两个就是温度
	a=ReadOneChar();
	b=ReadOneChar();
	
	//U1_send(b);
	//U1_send(a);
	//U1_send(' ');
		
	tmp = b;
	tmp<<=8;
	tmp |= a;	//组合温度
	
	Temperature = (tmp&(~0xF800))*0.0625;	
	return (tmp&0xF800)?-Temperature:Temperature;
}


