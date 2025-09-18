#include <LPC214x.h>
#include <stdio.h>
#include <stdlib.h>

#define RS (1 << 16)
#define EN (1 << 17)
#define D4 (1 << 18)
#define D5 (1 << 19)
#define D6 (1 << 20)
#define D7 (1 << 21)

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 6000; j++);
}

void LCD_Enable() {
    IO0SET = EN;
    delay_ms(1);
    IO0CLR = EN;
    delay_ms(1);
}

void LCD_Send4Bit(unsigned char data) {
    IO0CLR = D4 | D5 | D6 | D7;
    if (data & 1) IO0SET = D4;
    if (data & 2) IO0SET = D5;
    if (data & 4) IO0SET = D6;
    if (data & 8) IO0SET = D7;
    LCD_Enable();
}

void LCD_Cmd(unsigned char cmd) {
    IO0CLR = RS;
    LCD_Send4Bit(cmd >> 4);
    LCD_Send4Bit(cmd & 0x0F);
    delay_ms(2);
}

void LCD_Data(unsigned char data) {
    IO0SET = RS;
    LCD_Send4Bit(data >> 4);
    LCD_Send4Bit(data & 0x0F);
    delay_ms(2);
}

void LCD_Init() {
    delay_ms(20);
    LCD_Cmd(0x02);
    LCD_Cmd(0x28); 
    LCD_Cmd(0x0C);
    LCD_Cmd(0x06);
    LCD_Cmd(0x01); 
}

void LCD_SetCursor(unsigned char row, unsigned char col) {
    LCD_Cmd((row == 0 ? 0x80 : 0xC0) + col);
}

void LCD_String(char *str) {
    while (*str) LCD_Data(*str++);
}

void UART0_Init() {
    PINSEL0 |= 0x05; 
    U0LCR = 0x83;    
    U0DLM = 0;
    U0DLL = 97;     
    U0LCR = 0x03;    
}

void UART0_TxChar(char ch) {
    while (!(U0LSR & 0x20));
    U0THR = ch;
}

void UART0_TxString(char *str) {
    while (*str) UART0_TxChar(*str++);
}

void ADC1_Init() {
    PINSEL1 |= (1 << 18); 
    AD1CR = (1 << 1) | (4 << 8) | (1 << 21);
}

unsigned int ADC1_Read() {
    AD1CR |= (1 << 24);
    while (!(AD1GDR & (1UL << 31)));
    return (AD1GDR >> 6) & 0x3FF;
}

float Read_Temperature() {
    unsigned int val = ADC1_Read();
    return (val * 3.3 / 1023.0) * 100.0; 
}

void RTC_Init() {
    CCR = 0x02; 
    SEC = 0;
    MIN = 0;
    HOUR = 12;
    DOM = 18;
    MONTH = 9;
    YEAR = 2025;
    CCR = 0x01; 
}

void Get_Time(char *timeStr) {
    int h = HOUR;
    int m = MIN;
    int s = SEC;
    sprintf(timeStr, "%02d:%02d:%02d", h, m, s);
}

int main() {
    char buffer[32];
    float temp;
    char timeStr[16];

    IO0DIR |= RS | EN | D4 | D5 | D6 | D7; 

    LCD_Init();
    UART0_Init();
    ADC1_Init();
    RTC_Init();

    while (1) {
        temp = Read_Temperature();
        //Get_Time(timeStr);

        //LCD_SetCursor(0, 0);
        //sprintf(buffer, "Temp: %.1f C", temp);
        //LCD_String("                "); 
        //LCD_SetCursor(0, 0);
        //LCD_String(buffer);

        //LCD_SetCursor(1, 0);
        //LCD_String("Time: ");
        //LCD_String(timeStr);

        sprintf(buffer, "Temp: %d C | Time: %s\r\n", (int)temp, timeStr);
        UART0_TxString(buffer);

        delay_ms(1000);
    }
}
