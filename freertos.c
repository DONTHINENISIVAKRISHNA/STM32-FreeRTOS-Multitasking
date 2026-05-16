/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
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

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE BEGIN Application */

void StartLEDTask(void const * argument)
{
    uint32_t toggleCount = 0;
    printf("[LED_TASK]     Started. Blinking every 500ms.\r\n");

    for (;;)
    {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        toggleCount++;

        if (toggleCount % 10 == 0)
        {
            printf("[LED_TASK]     Toggle #%lu | LED %s\r\n",
                   toggleCount,
                   HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin) ? "ON " : "OFF");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void StartCounterTask(void const * argument)
{
    uint32_t counter = 0;
    printf("[COUNTER_TASK] Started. Printing every 1000ms.\r\n");

    for (;;)
    {
        counter++;
        printf("[COUNTER_TASK] Count: %5lu | Time: %lu ms\r\n",
               counter,
               (uint32_t)xTaskGetTickCount());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void StartUptimeTask(void const * argument)
{
    uint32_t uptimeSeconds = 0;
    printf("[UPTIME_TASK]  Started. Printing every 2000ms.\r\n");
    printf("-------------------------------------------\r\n");
    printf("  SIVA KRISHNA DONTHINENI - Module 7A     \r\n");
    printf("-------------------------------------------\r\n");

    for (;;)
    {
        uptimeSeconds += 2;
        UBaseType_t watermark = uxTaskGetStackHighWaterMark(NULL);
        printf("[UPTIME_TASK]  Uptime: %4lu sec | Stack free: %u words\r\n",
               uptimeSeconds,
               (unsigned int)watermark);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

/* USER CODE END Application */

