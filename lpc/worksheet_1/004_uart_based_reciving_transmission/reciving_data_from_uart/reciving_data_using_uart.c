#include<lpc21xx.h>
unsigned char data;
int main()
{
		PINSEL0|=0X00000005;
		U0LCR=0X83;
		U0DLL=97;
		U0LCR=0X03;
	while(1)
	{
			while(!(U0LSR&0X01));
		data=U0RBR;
		while(!(U0LSR &0X20));
		U0THR=data;
	}
}
