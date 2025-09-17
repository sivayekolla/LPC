#include <lpc214x.h>

#define LED1 (1 << 16)
#define LED2 (1 << 17)
#define LED3 (1 << 18)
#define LED4 (1 << 19)

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 6000; j++);  
}

void init_GPIO() {
    IO0DIR |= (LED1 | LED2 | LED3 | LED4);
}

void single_led_blink() {
    IO0SET = LED1;
    delay_ms(500);
    IO0CLR = LED1;
    delay_ms(500);
}

void running_lights() {
    IO0SET = LED1; delay_ms(100); IO0CLR = LED1;
    IO0SET = LED2; delay_ms(100); IO0CLR = LED2;
    IO0SET = LED3; delay_ms(100); IO0CLR = LED3;
    IO0SET = LED4; delay_ms(100); IO0CLR = LED4;

    IO0SET = LED3; delay_ms(100); IO0CLR = LED3;
    IO0SET = LED2; delay_ms(100); IO0CLR = LED2;
}

void alternating_pattern() {
    IO0SET = (LED1 | LED3);
    IO0CLR = (LED2 | LED4);
    delay_ms(500);

    IO0SET = (LED2 | LED4);
    IO0CLR = (LED1 | LED3);
    delay_ms(500);
}

int main() {
    init_GPIO();

    while (1) {
        single_led_blink();        // Pattern 1
        running_lights();          // Pattern 2
        alternating_pattern();     // Pattern 3
    }
}
