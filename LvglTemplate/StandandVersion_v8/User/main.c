#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "tftlcd.h"
#include "malloc.h" 
#include "sram.h"
#include "flash.h"
#include "ff.h" 
#include "fatfs_app.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "font_show.h"
#include "timer.h"
#include "app.h"
#include "sram.h"


int main()
{
	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	LED_Init();
	USART1_Init(115200);
	FSMC_SRAM_Init();
	printf("SRAM Init OK\r\n");
	my_mem_init(SRAMIN);		//初始化内部内存池
	printf("MEMINNER Init OK\r\n");
	TIM4_Init(1000-1, 72-1);//72Mhz
	printf("LVGL TIME BASE Init OK\r\n");
	while(font_init()==1)
	{
		printf("font_init err\r\n");
	}
	printf("font_init successful\r\n");
	lv_init(); 
	lv_port_disp_init(); 
	lv_port_indev_init();
	//SDcardFileViewerStart("0:");


	while(1)
	{
		delay_ms(5);
		lv_timer_handler();
	}
}
