#include"timer.h"
#include"led.h"
#include "lvgl.h"
// 72 * 1000 /72Mhz 
void TIM4_Init(u16 per,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitTypeDefStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	TIM_TimeBaseInitTypeDefStructure.TIM_ClockDivision =TIM_CKD_DIV1;
	TIM_TimeBaseInitTypeDefStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitTypeDefStructure.TIM_Period =per;
	TIM_TimeBaseInitTypeDefStructure.TIM_Prescaler=psc;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitTypeDefStructure);
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	
	
	NVIC_InitStructure.NVIC_IRQChannel =TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;	
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM4,ENABLE);
	
	
}

void TIM4_IRQHandler(void)
{
	static u16 i =0;
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET)
	{
		if(i%500 ==0){
			LED2 =!LED2;
		
		}
		lv_tick_inc(1);
		i++;

	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	
}

