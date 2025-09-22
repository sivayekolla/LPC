#include <LPC214x.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void init_pll(void);
void init_uart(unsigned int baudrate);
void uart_tx_string(const char *str);
void task_producer(void *pvParameters);
void task_consumer(void *pvParameters);

SemaphoreHandle_t counting_sem;

int main(void)
{
    init_pll();
    init_uart(9600);
    uart_tx_string("System Initialized\r\n");
       
    // Create counting semaphore with max count 5, initial count 0
    counting_sem = xSemaphoreCreateCounting(5, 0);
    if (counting_sem == NULL)
    {
        uart_tx_string("Counting semaphore creation failed!\r\n");
        while (1); // Stay here if semaphore creation fails
    }

    // Create tasks
    xTaskCreate(task_producer, "Producer", 256, NULL, 1, NULL);
    xTaskCreate(task_consumer, "Consumer", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1); // Should never reach here
}

// === Producer Task ===
void task_producer(void *pvParameters)
{
    while (1)
    {
        uart_tx_string("Producing signal...\r\n");
        xSemaphoreGive(counting_sem);  // Signal consumer
        vTaskDelay(pdMS_TO_TICKS(1000)); // Wait 1 sec
    }
}

// === Consumer Task ===
void task_consumer(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(counting_sem, portMAX_DELAY) == pdTRUE)
        {
            uart_tx_string("Consumed signal\r\n");
        }
        vTaskDelay(pdMS_TO_TICKS(500)); // Short delay to simulate work
    }
}

// === PLL Initialization ===
void init_pll(void)
{
    PLL0CON = 0x01;
    PLL0CFG = 0x24; // M = 5, P = 2 (12MHz * 5 = 60MHz)
    PLL0FEED = 0xAA;
    PLL0FEED = 0x55;
    while (!(PLL0STAT & (1 << 10))); // Wait for lock

    PLL0CON = 0x03; // Enable & connect
    PLL0FEED = 0xAA;
    PLL0FEED = 0x55;

    VPBDIV = 0x01; // PCLK = CCLK
}

// === UART Initialization ===
void init_uart(unsigned int baudrate)
{
    unsigned long pclk = 15000000;
    unsigned int divisor = pclk / (16 * baudrate);

    PINSEL0 &= ~0x0000000F;
    PINSEL0 |= 0x00000005;

    U0LCR = 0x83;
    U0DLL = divisor & 0xFF;
    U0DLM = (divisor >> 8) & 0xFF;
    U0LCR = 0x03;
    U0FCR = 0x07;
}

// === UART Send String ===
void uart_tx_string(const char *str)
{
    while (*str)
    {
        while (!(U0LSR & 0x20));
        U0THR = *str++;
    }
}
