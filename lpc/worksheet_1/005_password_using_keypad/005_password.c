#include <lpc214x.h>
#include <string.h>

// LCD Pin Definitions
#define RS (1 << 8)
#define EN (1 << 9)
#define D4 (1 << 10)
#define D5 (1 << 11)
#define D6 (1 << 12)
#define D7 (1 << 13)

// Keypad Row Pins
#define R1 (1 << 16)
#define R2 (1 << 17)
#define R3 (1 << 18)
#define R4 (1 << 19)

// Keypad Column Pins
#define C1 (1 << 20)
#define C2 (1 << 21)
#define C3 (1 << 22)
#define C4 (1 << 23)

// Keypad layout
char keypad[4][4] = {
    {'7','8','9','/'},
    {'4','5','6','*'},
    {'1','2','3','-'},
    {'C','0','=','+'}
};

// Delay
void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 6000; j++);
}

// LCD functions
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
        IO0SET = RS;
    else
        IO0CLR = RS;
    lcd_send_nibble((value >> 4) & 0x0F);
    lcd_send_nibble(value & 0x0F);
    delay_ms(2);
}

void lcd_cmd(unsigned char cmd) {
    lcd_send(cmd, 0);
}

void lcd_data(unsigned char data) {
    lcd_send(data, 1);
}

void lcd_init() {
    IO0DIR |= RS | EN | D4 | D5 | D6 | D7;
    delay_ms(20);
    lcd_send_nibble(0x03); delay_ms(5);
    lcd_send_nibble(0x03); delay_ms(5);
    lcd_send_nibble(0x03); delay_ms(5);
    lcd_send_nibble(0x02); // 4-bit mode
    lcd_cmd(0x28); // 4-bit, 2-line, 5x8
    lcd_cmd(0x0C); // Display ON, cursor OFF
    lcd_cmd(0x06); // Entry mode
    lcd_cmd(0x01); // Clear display
    delay_ms(5);
}

void lcd_goto(unsigned char row, unsigned char col) {
    if(row == 1)
        lcd_cmd(0x80 + col);
    else
        lcd_cmd(0xC0 + col);
}

void lcd_print(char *str) {
    while(*str)
        lcd_data(*str++);
}

// Keypad scan
char keypad_scan() {
    int row, col;
    IO1DIR = R1 | R2 | R3 | R4;         // Rows output
    IO1CLR = R1 | R2 | R3 | R4;         // Clear all rows
    IO1DIR &= ~(C1 | C2 | C3 | C4);     // Columns input

    for(row = 0; row < 4; row++) {
        IO1SET = R1 | R2 | R3 | R4;     // Set all rows HIGH
        IO1CLR = (1 << (16 + row));    // Clear one row at a time
        delay_ms(2);
        for(col = 0; col < 4; col++) {
            if(!(IO1PIN & (1 << (20 + col)))) {
                while(!(IO1PIN & (1 << (20 + col)))); // Wait for release
                return keypad[row][col];
            }
        }
    }
    return 0; // No key pressed
}

int main() {
    char key;
    char entered[5];     // To hold 4 digits + null terminator
    int idx = 0;
    char correct_password[] = "1316";

    lcd_init();
    lcd_goto(1, 0);
    lcd_print("Enter Password:");
    lcd_goto(2, 0);

    while(1) {
        key = keypad_scan();
        if(key) {
            // Accept only digits 0–9
            if(key >= '0' && key <= '9') {
                if(idx < 4) {
                    entered[idx++] = key;
                    lcd_data('*'); // Show * instead of digit
                }
            }

            // On '=' check the password
            else if(key == '=') {
                entered[idx] = '\0'; // Null-terminate

                lcd_cmd(0x01); // Clear screen
                lcd_goto(1, 0);

                if(strcmp(entered, correct_password) == 0) {
                    lcd_print("Correct Password");
                } else {
                    lcd_print("Wrong Password");
                }

                delay_ms(2000);
                lcd_cmd(0x01);
                lcd_goto(1, 0);
                lcd_print("Enter Password:");
                lcd_goto(2, 0);
                idx = 0;
            }

            // Clear (on 'C' press)
            else if(key == 'C') {
                idx = 0;
                lcd_cmd(0x01);
                lcd_goto(1, 0);
                lcd_print("Enter Password:");
                lcd_goto(2, 0);
            }
        }
    }
}
