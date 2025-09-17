#include <lpc214x.h>

#define LED1 (1 << 16)  // MSB
#define LED2 (1 << 17)
#define LED3 (1 << 18)
#define LED4 (1 << 19)  // LSB
#define BUTTON (1 << 10)

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 6000; j++);
}

void init_GPIO() {
    IO0DIR |= (LED1 | LED2 | LED3 | LED4); // LEDs as output
    IO0DIR &= ~BUTTON;                     // Button as input
}

unsigned char read_button_debounced() {
    static unsigned char last_state = 0;
    static unsigned int debounce_count = 0;
    unsigned char current_state = (IO0PIN & BUTTON) ? 1 : 0;

    if (current_state != last_state) {
        debounce_count++;
        if (debounce_count >= 10) {
            last_state = current_state;
            debounce_count = 0;
            if (current_state == 1)
                return 1;  // Valid press
        }
    } else {
        debounce_count = 0;
    }
    return 0;
}

void show_binary(unsigned int value) {
    IO0CLR = (LED1 | LED2 | LED3 | LED4); // Turn off all LEDs

    if (value & 0x08) IO0SET |= LED1; // Bit 3 ? P0.16
    if (value & 0x04) IO0SET |= LED2; // Bit 2 ? P0.17
    if (value & 0x02) IO0SET |= LED3; // Bit 1 ? P0.18
    if (value & 0x01) IO0SET |= LED4; // Bit 0 ? P0.19
}

int main() {
    unsigned int count = 0;

    init_GPIO();

    while (1) {
        // Blink current binary value
        show_binary(count);
        delay_ms(300);
        IO0CLR = (LED1 | LED2 | LED3 | LED4);
        delay_ms(300);

        // If button pressed, update count
        if (read_button_debounced()) {
            count++;
            if (count > 15) count = 0; // Wrap after 15
        }
    }
}
