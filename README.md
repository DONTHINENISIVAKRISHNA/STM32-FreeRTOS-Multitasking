# STM32-FreeRTOS-Multitasking
FreeRTOS multitasking firmware on STM32 Nucleo-F103RB- 3 Concurrent tasks with UART output
# STM32 FreeRTOS Multitasking — Nucleo-F103RB

## Overview

This project demonstrates **real-time multitasking firmware** on the STM32 Nucleo-F103RB using FreeRTOS. Three independent tasks run concurrently on a single ARM Cortex-M3 core, managed by the FreeRTOS scheduler. Output is transmitted live to a PC terminal via UART2 at 115200 baud.

The core concept: the FreeRTOS scheduler switches between 3 tasks every ~12 microseconds, making them **appear to run simultaneously** , a fundamental skill in all production embedded firmware.


## Hardware
Hardware components used and their description.

 Board - STM32 Nucleo-F103RB 
 Microcontroller-STM32F103RBT6 — ARM Cortex-M3 @ 64MHz 
 Flash - 128 KB 
 RAM - 20 KB 
 Onboard LED - LD2 Green — PA5 
 Debugger - ST-Link V2-1 (onboard)
IDE - STM32CubeIDE 2.1.1 
 RTOS - FreeRTOS via CMSIS-RTOS V1 

---

## What Are the 3 Concurrent Tasks?

### Task 1 — LEDTask
- Blinks the onboard **LD2 green LED (PA5)** every **500ms**
- Prints a toggle message every 10 blinks to avoid flooding the terminal
- Uses `vTaskDelay(pdMS_TO_TICKS(500))` to release CPU during the wait

```c
void StartLEDTask(void const * argument)
{
    uint32_t toggleCount = 0;
    for (;;)
    {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        toggleCount++;
        if (toggleCount % 10 == 0)
        {
            printf("[LED_TASK] Toggle #%lu | LED %s\r\n", toggleCount,
                   HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin) ? "ON" : "OFF");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
```

### Task 2 — CounterTask
- Increments a counter and prints it with **FreeRTOS tick time** every **1000ms**
- `xTaskGetTickCount()` returns milliseconds elapsed since scheduler started

```c
void StartCounterTask(void const * argument)
{
    uint32_t counter = 0;
    for (;;)
    {
        counter++;
        printf("[COUNTER_TASK] Count: %5lu | Time: %lu ms\r\n",
               counter, (uint32_t)xTaskGetTickCount());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

### Task 3 — UptimeTask
- Prints **system uptime** every **2000ms**
- Monitors stack health using `uxTaskGetStackHighWaterMark()` — a production diagnostic tool
- Returns minimum free stack words ever seen — near 0 means stack overflow danger

```c
void StartUptimeTask(void const * argument)
{
    uint32_t uptimeSeconds = 0;
    for (;;)
    {
        uptimeSeconds += 2;
        UBaseType_t watermark = uxTaskGetStackHighWaterMark(NULL);
        printf("[UPTIME_TASK] Uptime: %4lu sec | Stack free: %u words\r\n",
               uptimeSeconds, (unsigned int)watermark);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

## How the Microcontroller Runs 3 Tasks Simultaneously

The STM32F103RB has **one CPU core** , it cannot truly run 3 things at the same time. FreeRTOS solves this using a **preemptive scheduler**.

### How It Works

```
TIME(ms):    0      500     1000     1500     2000
             |       |        |        |        |
LEDTask:   [RUN]--BLOCKED--[RUN]--BLOCKED----[RUN]
CounterTask: [RUN]----------BLOCKED(1000ms)--[RUN]
UptimeTask:  [RUN]--------------------BLOCKED(2000ms)----[RUN]
```

1. All 3 tasks start and immediately call `vTaskDelay()`
2. Each task enters **BLOCKED state** — CPU is released
3. Scheduler picks the next READY task and runs it
4. When a task's delay expires — it moves back to READY
5. Scheduler runs it again at next opportunity
6. Context switch happens in **~12 microseconds**

### Task States

Task states and their Meaning.
RUNNING -On the CPU right now 
READY -Wants CPU — waiting its turn 
BLOCKED -Sleeping — waiting for delay or event. CPU is FREE 
SUSPENDED -Manually paused — ignored by scheduler 

---
HAL_Delay(500)
HAL_Delay is a simple blocking delay function provided by the STM32 HAL library. When you call HAL_Delay(500), the CPU sits in a tight loop doing nothing for 500 milliseconds. During this entire time the CPU is occupied ,it cannot do anything else. No other task can run, no other operation can execute. Everything freezes and waits until the 500ms is over. This is called a blocking delay because it blocks the entire CPU.
In a bare-metal single task program this is acceptable. But inside FreeRTOS with multiple tasks, using HAL_Delay is a serious mistake because one task holding the CPU for 500ms means all other tasks are completely frozen during that time. Timings break, responsiveness is lost, and the whole point of having an RTOS is defeated.
vTaskDelay(pdMS_TO_TICKS(500))
vTaskDelay is the FreeRTOS-aware delay function. When a task calls vTaskDelay(pdMS_TO_TICKS(500)), that task immediately moves from the RUNNING state to the BLOCKED state and releases the CPU completely. The FreeRTOS scheduler then picks the next available task and runs it. After 500ms the original task moves back to READY state and gets the CPU again when its turn comes.
The CPU is never wasted ,it is always doing useful work in another task while the first task is sleeping. This is called a non-blocking delay because the task sleeps without blocking the CPU.
pdMS_TO_TICKS() is a macro that converts milliseconds into FreeRTOS tick counts, making the delay value independent of the tick rate configuration.

---

## FreeRTOS Tools Used and their purpose.

osThreadDef() -Define a task with name, priority, stack size osThreadCreate()` -Create and start a task 
vTaskDelay() - Block task for a time period — releases CPU 
pdMS_TO_TICKS()- Convert milliseconds to FreeRTOS ticks 
xTaskGetTickCount() - Get current FreeRTOS time in milliseconds 
uxTaskGetStackHighWaterMark() - Monitor minimum free stack — detect overflow danger 
vApplicationGetIdleTaskMemory()-Provide static memory for FreeRTOS Idle Task 

---

## Communication Protocols

### UART (Universal Asynchronous Receiver Transmitter)
- **UART2** used to send `printf` output from STM32 to PC terminal
- No external wiring needed — Nucleo board routes UART2 through onboard ST-Link USB
- `__io_putchar()` redirects `printf` to UART2 byte by byte

```c
int __io_putchar(int ch) {
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 10);
    return ch;
}
```
 UART2 Setting and their values
TX Pin -PA2 
RX Pin -PA3 
Baud Rate -115200 
Word Length - 8 bits 
Stop Bits - 1 
Parity - None 

### SWD (Serial Wire Debug)
- ST-Link communicates with STM32 via SWD over 2 wires (SWDIO + SWCLK)
- Used for flashing firmware and debugging
- Not used for data output — UART handles that

## Key CubeMX Configuration Decisions

### Why TIM1 as Timebase?
FreeRTOS uses SysTick for its own 1ms scheduler tick. HAL also needs a tick source. If both use SysTick — **CONFLICT = HardFault crash on startup**. Moving HAL timebase to TIM1 prevents this. This must be done in every FreeRTOS project.

### Why USE_NEWLIB_REENTRANT Enabled?
Without it, `printf` uses a shared internal buffer — not thread-safe. When 3 tasks call `printf` simultaneously, output gets corrupted. Enabling this gives each task its own `printf` buffer, making it thread-safe.

### Why INCLUDE_uxTaskGetStackHighWaterMark = 1?
This function is disabled by default to save memory. Enabling it allows UptimeTask to monitor its own stack health live — a production-level defensive programming technique.


## Expected PuTTY Output

==============================================
  STM32 FreeRTOS Workshop - Module 7A
  Tasks + Scheduler Basics
  SIVA KRISHNA DONTHINENI
==============================================

[LED_TASK]     Started. Blinking every 500ms.
[COUNTER_TASK] Started. Printing every 1000ms.
[UPTIME_TASK]  Started. Printing every 2000ms.
-------------------------------------------
  SIVA KRISHNA DONTHINENI - Module 7A
-------------------------------------------
[COUNTER_TASK] Count:     1 | Time:  1001 ms
[UPTIME_TASK]  Uptime:    2 sec | Stack free: 393 words
[COUNTER_TASK] Count:     2 | Time:  2001 ms
[LED_TASK]     Toggle #10 | LED ON
[COUNTER_TASK] Count:     3 | Time:  3001 ms
[UPTIME_TASK]  Uptime:    4 sec | Stack free: 393 words

## Real World Applications

- **Automotive** — BMS, ADAS, Body Control Modules with multiple concurrent tasks
- **Medical Devices** — Patient monitors with separate tasks for each vital sign
- **IoT / Industrial** — Sensor reading + communication + diagnostics running simultaneously

## Author

**Siva Krishna Donthineni**
M.S. Computer Engineering, Arizona State University
Embedded Firmware & Software Engineer ,STM32 · FreeRTOS · C/C++ · ARM Cortex-M

