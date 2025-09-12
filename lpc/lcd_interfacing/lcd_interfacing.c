#include <lpc214x.h>
// Define LCD pins
#define RS (1 << 8)   // P0.8
#define EN (1 << 9)   // P0.9
#define D4 (1 << 10)  // P0.10
#define D5 (1 << 11)  // P0.11
#define D6 (1 << 12)  // P0.12
#define D7 (1 << 13)  // P0.13
void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 6000; j++);
}
// Send enable pulse
void lcd_enable_pulse() {
    IO0SET = EN;
    delay_ms(1);
    IO0CLR = EN;
    delay_ms(1);
}

void lcd_send_nibble(unsigned char nibble) {
    IO0CLR = D4 | D5 | D6 | D7;

    if(nibble & 0x01) IO0SET |= D4;
    if(nibble & 0x02) IO0SET |= D5;
    if(nibble & 0x04) IO0SET |= D6;
    if(nibble & 0x08) IO0SET |= D7;

    lcd_enable_pulse();
}

void lcd_send(unsigned char value, int is_data) {
    if(is_data)
        IO0SET = RS;  // RS = 1 for data
    else
        IO0CLR = RS;  // RS = 0 for command

    lcd_send_nibble((value >> 4) & 0x0F); // High nibble
    lcd_send_nibble(value & 0x0F);        // Low nibble

    delay_ms(2);
}

void lcd_init() {
    IO0DIR |= RS | EN | D4 | D5 | D6 | D7;  // Set pins as output
    delay_ms(20);           // Wait for LCD power up
    lcd_send_nibble(0x03);  // Initialize
    delay_ms(5);
    lcd_send_nibble(0x03);
    delay_ms(5);
    lcd_send_nibble(0x03);
    delay_ms(5);
    lcd_send_nibble(0x02);  // Set to 4-bit mode
    lcd_send(0x28, 0); // Function set: 4-bit, 2 lines
    lcd_send(0x0C, 0); // Display ON, cursor OFF
    lcd_send(0x06, 0); // Entry mode set: auto increment
    lcd_send(0x01, 0); // Clear display
    delay_ms(5);
}

void lcd_goto(unsigned char row, unsigned char col) {
    if(row == 1)
        lcd_send(0x80 + col, 0);
    else
        lcd_send(0xC0 + col, 0);
}

// Print a string
void lcd_print(char *str) {
    while(*str) {
        lcd_send(*str++, 1);
    }
}
int main() {
    lcd_init();  // Initialize LCD

    lcd_goto(1, 0);         // First row, 0th column
    lcd_print("Hello");

    lcd_goto(2, 0);         // Second row
    lcd_print("LPC2148");

    while(1);  // Loop forever
}
