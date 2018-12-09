/*
 * teperature_task.c
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
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "temperature_task.h"

#define PROFUNDIDADE_PILHA_TASK 128
#define MAX_TICKS 5
#define BUFFER_SIZE 10

extern xSemaphoreHandle g_pUARTSemaphore;
extern xSemaphoreHandle g_MasterSemaphore;
extern QueueHandle_t grava_Queue;
extern QueueHandle_t serial_Queue;

static void TemperatureTask(void* pvParameters){
    // Declare local variables
    uint32_t i = 0;                     // Counter variable
    uint32_t ui32ADC0Value[4];          // ADC12 read array
    volatile uint32_t ui32TempAvg;      // Temp Average
    volatile uint32_t ui32TempValueC;   // Temp in Celsius scale
    uint32_t wakeup = xTaskGetTickCount();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCHardwareOversampleConfigure(ADC0_BASE, 64);
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 1);

    while(1)
    {
        xSemaphoreTake(g_pUARTSemaphore, MAX_TICKS);
        UARTprintf("Task Temperatura acordou! \n");
        xSemaphoreGive(g_pUARTSemaphore);

        GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x2);

        ADCIntClear(ADC0_BASE, 1);
        ADCProcessorTrigger(ADC0_BASE, 1);

        while(!ADCIntStatus(ADC0_BASE, 1, false)){}

        ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);

        ui32TempAvg     = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
        ui32TempValueC  = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;

        xQueueSend(grava_Queue, (void *) &ui32TempValueC, MAX_TICKS);
        xQueueSend(serial_Queue, (void *) &ui32TempValueC, MAX_TICKS);

        vTaskDelayUntil(&wakeup, 1000 / portTICK_RATE_MS);
    }
}

uint32_t
TemperatureTaskInit(void){
    int status;
    status = xTaskCreate(
                    TemperatureTask,
                    (const portCHAR*)"TEMPERATURA_TASK",
                    PROFUNDIDADE_PILHA_TASK,
                    NULL,
                    tskIDLE_PRIORITY + TEMPERATURA_TASK_PRIORITY,
                    NULL
                    );

    if (status != pdTRUE){
        return 1;
    } else {
        return 0;
    }
}
