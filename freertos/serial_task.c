/*
 * serial_task.c
 *
 *  Created on: 2 de dez de 2018
 *      Author: Carlos
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "drivers/rgb.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "serial_task.h"

#define PROFUNDIDADE_PILHA_TASK 128
#define MAX_TICKS 5

extern xSemaphoreHandle g_pUARTSemaphore;
extern xSemaphoreHandle g_MasterSemaphore;

extern QueueHandle_t serial_Queue;

static void
SerialTask(void * pvParameters){
    while(1){
        uint32_t tmp;
        uint32_t buffer[10];
        uint32_t wakeup = xTaskGetTickCount();
        int i = 0;

        while(1) {
            if (xQueueReceive(serial_Queue, &tmp, MAX_TICKS) == pdPASS){
                buffer[i] = tmp;
                i++;
                continue;
            }

            if (i == 10){
                i = 0;
                GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x4);
                SysCtlDelay(20000000);

                xSemaphoreTake(g_pUARTSemaphore, MAX_TICKS);
                UARTprintf("Task Serial enviou: ");
                int j = 0;
                for(j = 0; j < 10; j++, UARTprintf("%d ", buffer[j]));
                UARTprintf("\n");
                xSemaphoreGive(g_pUARTSemaphore);
            }

            vTaskDelayUntil(&wakeup, 1000 / portTICK_RATE_MS);
        }
    }
}

uint32_t
SerialTaskInit(void){
    int status;
    status = xTaskCreate(
                    SerialTask,
                    (const portCHAR*)"SERIAL_TASK",
                    PROFUNDIDADE_PILHA_TASK,
                    NULL,
                    tskIDLE_PRIORITY + SERIAL_TASK_PRIORITY,
                    NULL
                    );

    if (status != pdTRUE){
        return 1;
    }
    else {
        return 0;
    }
}
