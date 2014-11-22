#include"Timer.h"
#include"stm32f10x.h"
#include"stdio.h"




void Timer2_Handle(void)
{
	printf("time2 test");
}



//5ms
void TIMER2_Handler(void)
{
  
    TIM2->SR = ~TIM_FLAG_Update;  //清除标志位
    #if 0
    if(Wifi_AP_OPEN_MODE)
	{
		Wifi_AP_OPEN_MODE++;
		if(Wifi_AP_OPEN_MODE == 50)
		{
            read_led_status =~read_led_status;    
			wifi_led(read_led_status);
			Wifi_AP_OPEN_MODE = 1;
		}
	}
	else
	{
		Wifi_MAC_Count++;
     
		if(Wifi_MAC_Count == 2*200) //3秒定时
		{
			Get_Wifi_MAC = 1;
			Wifi_MAC_Count = 0;
		}	
	}
    #else
    read_led_status =~read_led_status;    
//    wifi_led(read_led_status);
    #endif
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
    TIM_DeInit(TIM2);
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

void timer2_init(void)
{
     TIM2_NVIC_Configuration();
     TIM2_Configuration();
  //   timer2_disable();

}
void timer2_enable(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);	
}
void timer2_disable(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);
}
void isr_13us(void)
{
//    T++;
//    if(F)
//    {
//        if (Turn)
//        {
//            Turn=0;
//            SET_INFRARED;
//        }
//        else
//        {
//            Turn=1;
//            CLR_INFRARED;
//        }
//    }

}


//////////////////////////////////////////////////
//////////////////////////////////////////////////
#include  "time.h" 

#if 0


//定时器time0中断，时钟频率11.0592MHz，中断时间1ms
void Timer0Init(void)		
{
	AUXR |= 0x80;		
	TMOD &= 0xF0;		
	TMOD |= 0x01;		
	TL0 = 0xCD;		
	TH0 = 0xD4;		
	TF0 = 0;		
	TR0 = 1;		
}

  #endif
















