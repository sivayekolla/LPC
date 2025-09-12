#include <LPC214x.h>  // Include LPC2148 register definitions

#define LED_MASK (0x3FFU << 16)  // Binary: 00000011 11111111 00000000 00000000
                                 // Covers P1.16 to P1.25

void delay(unsigned int time);

int main(void) {
    // Set P1.16 to P1.25 as output
    IO1DIR |= LED_MASK;

    while (1) {
        IO1SET = LED_MASK;  // Turn ON LEDs (set bits 16–25)
        delay(10000);       // Wait

        IO1CLR = LED_MASK;  // Turn OFF LEDs (clear bits 16–25)
        delay(10000);       // Wait
    }
}

// Simple software delay function
void delay(unsigned int time) {
    unsigned int i, j;
    for (i = 0; i < time; i++) {
        for (j = 0; j < 1000; j++);
    }
}
