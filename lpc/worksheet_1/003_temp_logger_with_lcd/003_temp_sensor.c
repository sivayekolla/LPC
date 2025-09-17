#include <lpc214x.h>
#include <stdio.h>

#define RS (1 << 4)
#define EN (1 << 5)
#define D4 (1 << 6)
#define D5 (1 << 7)
#define D6 (1 << 8)
#define D7 (1 << 9)

void delay_ms(unsigned int ms);
void lcd_init(void);
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_print(char *str);
void lcd_set_cursor(unsigned char row, unsigned char col);
void lcd_send_nibble(unsigned char nibble);
void adc_init(void);
unsigned int adc_read(void);

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 6000; j++);
}

void lcd_send_nibble(unsigned char nibble) {
    IO0CLR = D4 | D5 | D6 | D7;
    if(nibble & 0x01) IO0SET |= D4;
    if(nibble & 0x02) IO0SET |= D5;
    if(nibble & 0x04) IO0SET |= D6;
    if(nibble & 0x08) IO0SET |= D7;
    IO0SET |= EN;
    delay_ms(1);
    IO0CLR |= EN;
}

void lcd_cmd(unsigned char cmd) {
    IO0CLR |= RS;
    lcd_send_nibble((cmd >> 4) & 0x0F);
    lcd_send_nibble(cmd & 0x0F);
    delay_ms(2);
}

void lcd_data(unsigned char data) {
    IO0SET |= RS;
    lcd_send_nibble((data >> 4) & 0x0F);
    lcd_send_nibble(data & 0x0F);
    delay_ms(2);
}

void lcd_init() {
    IO0DIR |= RS | EN | D4 | D5 | D6 | D7;
    delay_ms(20);
    lcd_send_nibble(0x03); delay_ms(5);
    lcd_send_nibble(0x03); delay_ms(5);
    lcd_send_nibble(0x03); delay_ms(5);
    lcd_send_nibble(0x02); delay_ms(1);
    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
    delay_ms(2);
}

void lcd_print(char *str) {
    while(*str) {
        lcd_data(*str++);
    }
}

void lcd_set_cursor(unsigned char row, unsigned char col) {
    unsigned char pos = (row == 1) ? (0x80 + col) : (0xC0 + col);
    lcd_cmd(pos);
}

void adc_init() {
    PINSEL1 |= (1 << 18);
    AD1CR = (1 << 1) | (4 << 8) | (1 << 21) | (1 << 24);
    delay_ms(10);
}

unsigned int adc_read() {
    AD1CR |= (1 << 24);
    while(!(AD1GDR & (1 << 31)));
    return (AD1GDR >> 6) & 0x3FF;
}

int main() {
    char buffer[16];
    unsigned int adc_value;
    float voltage, temp_c;

    adc_init();
    lcd_init();

    while(1) {
        adc_value = adc_read();
        voltage = (adc_value * 3.3) / 1024.0;
        temp_c = voltage * 100.0;

        lcd_set_cursor(1, 0);
        lcd_print("                ");
        lcd_set_cursor(1, 0);
        sprintf(buffer, "Temp: %.2f C", temp_c);
        lcd_print(buffer);
        delay_ms(1000);
    }
}
