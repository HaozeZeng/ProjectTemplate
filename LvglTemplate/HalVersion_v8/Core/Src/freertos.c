/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "SysTick.h"
#include "lvgl.h"
#include "key.h"
#include "app.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
        .name = "defaultTask",
        .stack_size = 128 * 4,
        .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Led */
osThreadId_t LedHandle;
const osThreadAttr_t Led_attributes = {
        .name = "Led",
        .stack_size = 256 * 4,
        .priority = (osPriority_t) osPriorityBelowNormal7,
};
/* Definitions for LvglTask */
osThreadId_t LvglTaskHandle;
const osThreadAttr_t LvglTask_attributes = {
        .name = "LvglTask",
        .stack_size = 2048 * 4,
        .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void LedTask(void *argument);

void LvglCreate(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* creation of Led */
    LedHandle = osThreadNew(LedTask, NULL, &Led_attributes);

    /* creation of LvglTask */
    LvglTaskHandle = osThreadNew(LvglCreate, NULL, &LvglTask_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */

    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument) {
    /* USER CODE BEGIN StartDefaultTask */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_LedTask */
/**
* @brief Function implementing the Led thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LedTask */
void LedTask(void *argument) {
    /* USER CODE BEGIN LedTask */
    /* Infinite loop */
    for (;;) {
        LED2 = !LED2;
        printf("KeyTask remain %lu \r\n", uxTaskGetStackHighWaterMark(LvglTaskHandle));
        printf("LED_Task remain %lu \r\n", uxTaskGetStackHighWaterMark(NULL));
        osDelay(500);
    }
    /* USER CODE END LedTask */
}

/* USER CODE BEGIN Header_LvglCreate */
/**
* @brief Function implementing the LvglTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LvglCreate */
void LvglCreate(void *argument) {
    /* USER CODE BEGIN LvglCreate */
    /* Infinite loop */
    SDcardFileViewerStart("0:", SanJiBang_16);
    for (;;) {
        lv_timer_handler();
        osDelay(pdMS_TO_TICKS(5));
    }
    /* USER CODE END LvglCreate */
}

/* Private application code --------------------------------------------------*/



/* USER CODE END Application */

