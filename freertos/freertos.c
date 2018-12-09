
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/eeprom.h"
#include "utils/uartstdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "tasks/grava_task.h"
#include "tasks/temperature_task.h"
#include "tasks/serial_task.h"

xSemaphoreHandle g_pUARTSemaphore;
xSemaphoreHandle g_MasterSemaphore;
QueueHandle_t grava_Queue;
QueueHandle_t serial_Queue;

#define MAX_QUEUE_LENGTH 10

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}

#endif

void
vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    while(1){}
}

void
ConfigureLED(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x00);

    SysCtlDelay(20000000);
}

void
ConfigureEEPROM(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    EEPROMInit();
    EEPROMMassErase();
}

void
ConfigureUART(void)
{
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    UARTStdioConfig(0, 115200, 16000000);
}

int
main(void)
{
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    ConfigureUART();
    ConfigureLED();
    ConfigureEEPROM();

    grava_Queue      = xQueueCreate(MAX_QUEUE_LENGTH, sizeof(uint32_t));
    serial_Queue     = xQueueCreate(MAX_QUEUE_LENGTH, sizeof(uint32_t));
    g_pUARTSemaphore = xSemaphoreCreateMutex();

    if (GravaTaskInit() == 1) {
        while(1){};
    }

    if (TemperatureTaskInit() == 1) {
        while(1){};
    }

    if (SerialTaskInit() == 1) {
        while(1){};
    }

    UARTprintf("\n-- Projeto de Sistemas Operacionais --\n\n");
    vTaskStartScheduler();

    while(1){}
}
