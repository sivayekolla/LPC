#include <LPC21xx.h>  

#define LED (1 << 15)  // Define LED on P0.15

void init_timer0(void) {
    T0PR = 59999;     
    T0TCR = 0x02;      // Reset Timer
    T0TCR = 0x01;      // Enable Timer
}

void delay_ms(unsigned int ms) {
    T0TC = 0;              // Reset the Timer Counter
    while (T0TC < ms);     
}

int main(void) {
    IODIR0 |= LED;         // Set P0.15 as output

    init_timer0();        

    while (1) {
        IOSET0 = LED;      // LED ON
        delay_ms(500);     

        IOCLR0 = LED;      // LED OFF
        delay_ms(500);     // Delay 500 ms
    }
}
