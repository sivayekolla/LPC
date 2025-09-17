#include <lpc214x.h>

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 6000; j++);
}

void pwm_init() {
    PINSEL0 |= (2 << 16);  

    PWMPR = 15;           
    PWMMR0 = 1000;       
    PWMMR1 = 500;        

    PWMMCR = (1 << 1);    
    PWMPCR = (1 << 9);  
    PWMLER = (1 << 0) | (1 << 1); 
    PWMTCR = (1 << 1);   
    PWMTCR = (1 << 0) | (1 << 3); 
}

void set_speed(unsigned int duty) {
    if (duty > 100) duty = 100;
    PWMMR1 = (duty * PWMMR0) / 100;  
    PWMLER = (1 << 1);             
}

int main() {
    pwm_init();

    while(1) {
				int i;
      for( i = 0; i <= 100; i += 10) {
            set_speed(i);
            delay_ms(500);
        }

        for( i = 100; i >= 0; i -= 10) {
            set_speed(i);
            delay_ms(500);
        }

        delay_ms(1000); 
    }
}
