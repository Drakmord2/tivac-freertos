/*
 * grava_task.c
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
#include "driverlib/eeprom.h"
#include "utils/uartstdio.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "grava_task.h"

#define PROFUNDIDADE_PILHA_TASK 128
#define MAX_TICKS 5

extern xSemaphoreHandle g_pUARTSemaphore;
extern xSemaphoreHandle g_MasterSemaphore;

extern QueueHandle_t grava_Queue;

static void
GravaTask(void* pvParameters){

    uint32_t tmp;
    uint32_t buffer[10];
    int i = 0;
    uint32_t wakeup = xTaskGetTickCount();

    while(1) {
        if (xQueueReceive(grava_Queue, &tmp, MAX_TICKS) == pdPASS){
            buffer[i] = tmp;
            i++;
            continue;
        }

        if (i == 10){
            GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x8);
            SysCtlDelay(20000000);

            i = 0;
            EEPROMProgram(buffer, 0x0, sizeof(buffer));

            uint32_t epromm[10];
            EEPROMRead(epromm, 0x0, sizeof(epromm));

            xSemaphoreTake(g_pUARTSemaphore, MAX_TICKS);
            UARTprintf("Task Grava gravou o dado! \n");
            xSemaphoreGive(g_pUARTSemaphore);
        }

        vTaskDelayUntil(&wakeup, 1000 / portTICK_RATE_MS);
    }
}

uint32_t
GravaTaskInit(void){
    int status;
    status = xTaskCreate(
                    GravaTask,
                    (const portCHAR*)"GRAVA_TASK",
                    PROFUNDIDADE_PILHA_TASK,
                    NULL,
                    tskIDLE_PRIORITY + GRAVA_TASK_PRIORITY,
                    NULL
                    );

    if (status != pdTRUE) {
        return 1;
    }
    else {
        return 0;
    }
}
