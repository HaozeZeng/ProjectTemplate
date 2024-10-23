#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"

//////////////////////////////////////////任务优先级堆栈分配//////////////////////////////////////////
#define START_TASK_PRIO			1
#define START_STACK_SIZE 		128  

#define TASK1_PRIO				2
#define TASK1_STACK_SIZE 		128  
//////////////////////////////////////////任务句柄//////////////////////////////////////////
TaskHandle_t 		Task1_Handler;
TaskHandle_t 		StartTask_Handler;
//////////////////////////////////////////任务函数声明//////////////////////////////////////////
void start_task	(void *pvParameters);
void task1		(void *pvParameters);


int main()
{
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	LED_Init();
	USART1_Init(115200);
	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STACK_SIZE,      //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          
}

//////////////////////////////////////////任务函数//////////////////////////////////////////

void task1(void *pvParameters)
{
    while(1)
    {
		printf("Hello,World!!!\r\n");
        LED1=!LED1;
        vTaskDelay(1000);
    }
}



void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    xTaskCreate((TaskFunction_t )task1,     
                (const char*    )"task1",   
                (uint16_t       )TASK1_STACK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&Task1_Handler); 
				
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
} 

