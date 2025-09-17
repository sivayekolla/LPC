#include <lpc214x.h>

#define RS (1<<8)
#define EN (1<<9)
#define D4 (1<<10)
#define D5 (1<<11)
#define D6 (1<<12)
#define D7 (1<<13)

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for(i=0; i<ms; i++)
        for(j=0; j<6000; j++);
}

// LCD functions
void lcd_enable() {
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
    lcd_enable();
}

void lcd_send_byte(unsigned char data, int is_data) {
    if(is_data) IO0SET = RS;
    else IO0CLR = RS;

    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0F);
    delay_ms(2);
}

void lcd_cmd(unsigned char cmd) {
    lcd_send_byte(cmd, 0);
}

void lcd_data(unsigned char data) {
    lcd_send_byte(data, 1);
}

void lcd_init() {
    IO0DIR |= RS | EN | D4 | D5 | D6 | D7;
    delay_ms(20);
    lcd_send_nibble(0x03); delay_ms(5);
    lcd_send_nibble(0x03); delay_ms(5);
    lcd_send_nibble(0x03); delay_ms(5);
    lcd_send_nibble(0x02);
    lcd_cmd(0x28); // 2-line mode
    lcd_cmd(0x0C); // Display ON
    lcd_cmd(0x06); // Auto increment
    lcd_cmd(0x01); // Clear
    delay_ms(5);
}

void lcd_goto(unsigned char row, unsigned char col) {
    lcd_cmd((row == 1 ? 0x80 : 0xC0) + col);
}

void lcd_print(char *str) {
    while(*str)
        lcd_data(*str++);
}

void int_to_str(int val, char *buf) {
    buf[0] = '0' + (val / 10);
    buf[1] = '0' + (val % 10);
    buf[2] = '\0';
}

// === RTC Functions ===

void rtc_init() {
    // Enable RTC peripheral
    PCONP |= (1 << 9);

    // Reset RTC
    CCR = 0x02;
    delay_ms(1);

    // Set 24-hour time: 21:45:00 (i.e., 9:45 PM railway time)
    HOUR = 21;   // 24-hour format
    MIN  = 45;
    SEC  = 0;

    // Set date: 17 Sept 2025
    DOM   = 17;
    MONTH = 9;
    YEAR  = 2025;

    // Enable RTC
    CCR = 0x01;
}

// === Month Name Lookup ===
char* get_month_name(int m) {
    char *months[] = {
        "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
        "JUL", "AUG", "SEPT", "OCT", "NOV", "DEC"
    };
    if (m >= 1 && m <= 12)
        return months[m - 1];
    else
        return "ERR";
}

int main() {
    char h[3], m[3], s[3], d[3], y[5];

    lcd_init();
    rtc_init();

    while(1) {
        int_to_str(HOUR, h);
        int_to_str(MIN, m);
        int_to_str(SEC, s);

        lcd_goto(1, 0);
        lcd_print("Time: ");
        lcd_print(h);
        lcd_data(':');
        lcd_print(m);
        lcd_data(':');
        lcd_print(s);

        if (SEC == 0 || SEC == 1) {
            int_to_str(DOM, d);
            lcd_goto(2, 0);
            lcd_print("Date: ");
            lcd_print(d);
            lcd_data(' ');
            lcd_print(get_month_name(MONTH));
            lcd_data(' ');

            // Year conversion
            y[0] = '2';
            y[1] = '0' + ((YEAR % 1000) / 100);
            y[2] = '0' + ((YEAR % 100) / 10);
            y[3] = '0' + (YEAR % 10);
            y[4] = '\0';
            lcd_print(y);
        } else {
            // Clear second line if not showing date
            lcd_goto(2, 0);
            lcd_print("                "); // 16 spaces
        }

        delay_ms(1000);  // Wait 1 second
    }
}
