#include <LPC214x.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void initPLL(void);
void initUART0(void);
void sendStringUART0(const char *str);

void task1(void *params);
void task2(void *params);

xSemaphoreHandle binarySem;

int main(void)
{
    initUART0();

    sendStringUART0("System Starting...\r\n");

    binarySem = xSemaphoreCreateBinary();
    if (binarySem == NULL) {
        sendStringUART0("Semaphore creation failed!\r\n");
        while(1);
    }

    xSemaphoreGive(binarySem);

    if (xTaskCreate(task1, "Task1", 128, NULL, 1, NULL) != pdPASS) {
        sendStringUART0("Task1 creation failed!\r\n");
    }

    if (xTaskCreate(task2, "Task2", 128, NULL, 1, NULL) != pdPASS) {
        sendStringUART0("Task2 creation failed!\r\n");
    }

    vTaskStartScheduler();

    sendStringUART0("Scheduler failed to start!\r\n");
    while(1);
}

void task1(void *params)
{
    while (1) {
        if (xSemaphoreTake(binarySem, portMAX_DELAY) == pdTRUE) {
            sendStringUART0("Task1 functioning\r\n");
            xSemaphoreGive(binarySem);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void task2(void *params)
{
    while (1) {
        if (xSemaphoreTake(binarySem, portMAX_DELAY) == pdTRUE) {
            sendStringUART0("Task2 functioning\r\n");
            xSemaphoreGive(binarySem);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void initPLL(void)
{
    PLL0CON = 0x01;
    PLL0CFG = 0x24;
    PLL0FEED = 0xAA;
    PLL0FEED = 0x55;

    while (!(PLL0STAT & (1 << 10)));

    PLL0CON = 0x03;
    PLL0FEED = 0xAA;
    PLL0FEED = 0x55;

    VPBDIV = 0x01;
}

void initUART0(void)
{
    PINSEL0 |= 0x00000005;
    U0LCR = 0x83;
    U0DLL = 0x61;
    U0DLM = 0x00;
    U0LCR = 0x03;
    U0FCR = 0x07;
}

void sendStringUART0(const char *str)
{
    while (*str) {
        while (!(U0LSR & 0x20));
        U0THR = *str++;
    }
}
