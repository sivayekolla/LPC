#include<lpc214x.h>
#include<string.h>
#define SLAVE_ADDR 0x70     
#define MAX        12
#define AA         2
#define SI         3
#define STO        4
#define STA        5
#define I2EN       6
int wait(int count){
      while(count-- );
}

void I2C_Init (void)
{
   VPBDIV    =   0x02;       
   PINSEL0   =   0x00000050; 
   
   I2C0SCLH   =   150;        
   I2C0SCLL   =   150;

   I2C0CONSET  =  (1<<I2EN);  
}

int I2C_Start ()
{
   I2C0CONCLR   =   1 << STO; 
   I2C0CONCLR = 1 << SI;   
   I2C0CONSET   =   1 << STA; 
   return 0;
}


void delay_ms(int count)
{
  int j=0,i=0;
 
  for(j=0;j<count;j++)
  {
    
    for(i=0;i<35;i++);
  }
}

void senddata(char data){ 
     while(!(I2C0CONSET & 0x08)); 		 
		 I2C0DAT      =   data; 
		 I2C0CONCLR   =   1 << SI;
		 delay_ms(200);
} 
void sendchar(char data){     
	 
	   senddata(0x50 | (data>>4));delay_ms(50);
	   senddata(0x40 | (data>>4));delay_ms(50);
		 senddata(0x50 | (data&0x0f)); delay_ms(50);
	   senddata(0x40 | (data& 0x0f));delay_ms(50);
		 delay_ms(50);
} 

int main(){
	
		  char code =  SLAVE_ADDR;
		  
		  char i2cdev[] = "LCD DISPLAY ON";
		  int i =0;

		  I2C_Init();                  	
			I2C_Start();                 
     	wait(4000);

			while(!(I2C0CONSET & 0x08));
			IO0SET = (1<<21);
      I2C0CONCLR	 =	 1 << STO;
			I2C0CONCLR	 =	 1 << STA;				
			I2C0CONSET   =   1 << AA;
			I2C0DAT      =  code;
			
			I2C0CONCLR   =   1 << SI; 
			while(!(I2C0CONSET & 0x08));
			if(I2C0STAT == 0x18)
			{	  
					 IO0SET = (1<<23);	  	   	 
				   I2C0CONSET   =   1 << AA;
					 I2C0DAT      =   0x00; 
				   I2C0CONCLR   =   1 << SI; 
				   while(!(I2C0CONSET & 0x08));
					 for(i=0; i < 2000;i++)wait(800);
				   if(I2C0STAT == 0x28)
					 {		 

						   
						 senddata(0x10);        
						 senddata(0x00);	       		 
						 		 
						 senddata(0x12);		  
						 senddata(0x02);				
						 
						 senddata(0x12);
						 senddata(0x02);
					
						 
						 senddata(0x18);
						 senddata(0x08);
					 
						 senddata(0x10);
						 senddata(0x00);
						 
						 senddata(0x1e);
						 senddata(0x0e);
								
						 senddata(0x10);
						 senddata(0x00);		  
						
						 senddata(0x16);
						 senddata(0x06);					
						
						 senddata(0x10);
						 senddata(0x00);
						
						 senddata(0x11);
						 senddata(0x01);				 
						
						 senddata(0x18);
						 senddata(0x08);	  
						
						 senddata(0x10);
						 senddata(0x00);
					
					
					   while(1){
							    i = 0;
							    while(i2cdev[i] !='\0'){
								       sendchar(i2cdev[i]);
								       i++;
							    }									
							    senddata(0x10);
							    senddata(0x00);							
							    senddata(0x11);
							    senddata(0x01);
						 }					
			    }			
			}
}