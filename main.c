// INDUSTRILA MONITORING SYSTEM
#include<lpc17xx.h>

#define IR (1<<19)
#define IN1 (1<<19)
#define IN2 (1<<16)
#define buzzer (1<<27)
#define gas (1<<20)
#define fire (1<<21)
#define relay (1<<28)

void delay(unsigned int i);
void LCD_INIT();
void cmd(unsigned char c);
void dat(unsigned char d);
void str(unsigned char arr[30]);
void helmetcheck(void);
void firecheck();
void gascheck();
void delay(unsigned int i);
void temperature_monitoring();

void main()
{
    LCD_INIT();
    LPC_SC->PCONP|=(1<<12); // sc-system control, enable power ADC
    LPC_SC->PCLKSEL0|=(1<<25)|(1<<24);// enable clk for adcc
    LPC_PINCON->PINSEL1|=(1<<18);//config P0.25 of AIN2 AD0.2
    LPC_ADC->ADCR=(1<<2)|(4<<8)|(1<<16)|(1<<21);
    LPC_GPIO1->FIODIR&=~IR;
    LPC_GPIO1->FIODIR&=~fire;
    LPC_GPIO1->FIODIR&=~gas;
    LPC_GPIO1->FIODIR|=IN1;
    LPC_GPIO1->FIODIR|=IN2;
    LPC_GPIO1->FIODIR|=relay;
    LPC_GPIO1->FIODIR|=buzzer;
    LPC_GPIO1->FIOCLR|=buzzer;
    LPC_GPIO1->FIOCLR|=buzzer;
    LPC_GPIO1->FIOCLR|=relay;
    cmd(0x01);
    cmd(0x80);
    str("INDUSTRIAL MONITOR");
    cmd(0xc0);
    str("SYSTEM");
    delay(1000);
    while(1)
    {
        helmetcheck();
        firecheck();
        gascheck();
    temperature_monitoring();
    }
}

void delay(unsigned int i)
{
    int j,k;
    for(j=0;j<i;j++)
    for(k=0;k<3000;k++);
}

void LCD_INIT()
{
    LPC_GPIO0->FIODIR=RS|EN|DATA;
    cmd(0x38);
    cmd(0x0e);
    cmd(0x01);
    cmd(0x80);
    cmd(0xC0);
}

void cmd(unsigned char c)
{
    LPC_GPIO0->FIOCLR=DATA;
    LPC_GPIO0->FIOSET=(c<<15);
    LPC_GPIO0->FIOCLR=RS;	//rs=0
    LPC_GPIO0->FIOSET=EN;
    delay(10);
    LPC_GPIO0->FIOCLR=EN;
    delay(10);
}

void dat(unsigned char d)
{
    LPC_GPIO0->FIOCLR=DATA;
    LPC_GPIO0->FIOSET=(d<<15);
    LPC_GPIO0->FIOSET=RS; //rs=1
    LPC_GPIO0->FIOSET=EN;
    delay(10);
    LPC_GPIO0->FIOCLR=EN;
    delay(10);
}

void str(unsigned char arr[30])
{
    int i;
    for(i=0;arr[i]!='\0';i++)
    {
        dat(arr[i]);
    }
}

void helmetcheck()
{
    if((LPC_GPIO1->FIOPIN&IR)==0)
    {
        cmd(0x01);
        cmd(0x80);
        str("helmet detected");
        LPC_GPIO1->FIOSET=IN1;
        LPC_GPIO1->FIOCLR=IN2;
        delay(1000);
        LPC_GPIO1->FIOCLR=IN1;
        LPC_GPIO1->FIOCLR=IN2;
        delay(1000);
        LPC_GPIO1->FIOCLR=IN1;
        LPC_GPIO1->FIOSET=IN2;
        delay(1000);
        LPC_GPIO1->FIOCLR=IN1;
        LPC_GPIO1->FIOCLR=IN2;
        delay(1000);
    }
    else
    {
    cmd(0x01);
    cmd(0x80);
    str("No helmet");
    delay(1000);
    LPC_GPIO1->FIOCLR=IN1;
    LPC_GPIO1->FIOCLR=IN2;
    delay(1000);
    }
}

void firecheck()
{
    if((LPC_GPIO1->FIOPIN&fire)==0)
    {
        cmd(0x01);
        cmd(0x80);
        str("fire detected");
        LPC_GPIO1->FIOSET=buzzer;
        delay(1000);
        LPC_GPIO1->FIOCLR=buzzer;
        delay(1000);
    }
    else
    {
        cmd(0x01);
        cmd(0x80);
        str("No fire");
        delay(1000);
        LPC_GPIO1->FIOCLR=buzzer;
        delay(1000);
    }
}

 void gascheck()
 {
    if((LPC_GPIO1->FIOPIN&gas)==0)
    {
        cmd(0x01);
        cmd(0x80);
        str("gas detected");
        LPC_GPIO1->FIOSET=buzzer;
        delay(1000);
        LPC_GPIO1->FIOCLR=buzzer;
        delay(1000);
    }
    else
    {
        cmd(0x01);
        cmd(0x80);
        str("No gas");
        delay(1000);
        LPC_GPIO1->FIOCLR=buzzer;
        delay(1000);
    }
}

void TEMP_MONITORING()
{
 unsigned int adc_value;
 while(!(LPC_ADC->ADGDR & (1<<31))); // CHECK DONE BIT IS SET OR NOT USING POLLING MECHANISM
 adc_value=(LPC_ADC->ADDR2 & (0xFFF<<4));//READ THE DIGITAL VALUE OF THE VARIABLE
 adc_value=adc_value<<4; // right shift by 4 to get lsb
 adc_value=adc_value/13.6;//convert raw temperature into degree centigrade
 cmd(0x01);
 cmd(0x80);
 string("TEMP:");
 cmd(0xC0);
 number(adc_value);//diaplay on the LCD
 dat(0xDF);
 string("c");
 delay(1000);
 if(adc_value>27)
 {
    cmd(0x01);
    string("TEMP IS HIGH");
    LPC_GPIO1->FIOSET=buzzer;
    LPC_GPIO1->FIOSET=relay;
    delay(1000);
 }
 else
 {
    cmd(0x01);
    string("NORMAL TEMP");
    LPC_GPIO1->FIOCLR=relay;
    delay(1000);
 }
}
