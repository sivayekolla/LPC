#include <LPC214x.h>
#include "FreeRTOS.h"
#include "task.h"

void UART0_Init(void) {
    VPBDIV = 0x01;
    PINSEL0 |= 0x00000005;
    U0LCR = 0x83;
    U0DLM = 0x00;
    U0DLL = 97;
    U0LCR = 0x03;
}

void UART0_SendChar(char c) {
    while (!(U0LSR & 0x20));
    U0THR = c;
}

void UART0_SendString(const char *s) {
    while (*s) UART0_SendChar(*s++);
}

void Task1(void *p) {
    UART0_SendString("Task1: started\r\n");
    while (1) {
        UART0_SendString("Task1 running\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void Task2(void *p) {
    UART0_SendString("Task2: started\r\n");
    while (1) {
        UART0_SendString("Task2 running\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void Task3(void *p) {
    UART0_SendString("Task3: started\r\n");
    while (1) {
        UART0_SendString("Task3 running\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void) {
    UART0_Init();
    UART0_SendString("Before scheduler\r\n");
    xTaskCreate(Task1, "T1", 256, NULL, 1, NULL);
    xTaskCreate(Task2, "T2", 256, NULL, 1, NULL);
    xTaskCreate(Task3, "T3", 256, NULL, 1, NULL);
    vTaskStartScheduler();
    while (1);
}
