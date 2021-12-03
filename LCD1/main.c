#include <p18cxxx.h>
#include <delays.h>
#include "LCD4lib.h" 

#pragma config FOSC = INTIO67   // Oscillator Selection bits (Internal oscillator block)
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)

//Functions prototypes:
void SetupIOs(void);
void ServeTimer0(void);
//char Return_Val(void);
//char Inc_Count(int count);
//char Dec_Count(int count1);
//unsigned char Delay2;
//unsigned char temp;
//void SetADCChannel(unsigned char);
unsigned char ReadADC_8(void);

//Global variables:
unsigned char SystemTick=0;
int Pump_Countdown = 0;
int Pump_Delay = 2; 
unsigned char F1;  
unsigned char F2; //Flag of button 2
unsigned char F3; //Flag of button 3
//int dec_val=0; //initial values
//int inc_val=0; //initial values

//Macros:
//Timing macros:
#define CUSTOM_TICKS_1      1   //every tick 
#define CUSTOM_TICKS_2      10  //every 10 ticks 
#define TICKS_PER_SECOND    20  //given 50ms interrupt @ 4MHz

//Misc:
/*#define TOGGLE_LED_1()      {LATCbits.LATC0 = !PORTCbits.RC0;}
#define TOGGLE_LED_2()      {LATCbits.LATC1 = !PORTCbits.RC1;}
*/
//Buttons:
#define B1 PORTEbits.RE0  //Button 1
#define B2 PORTAbits.RA0  //Button 2
#define B3 PORTAbits.RA1  //Button 3
#define PM1 PORTBbits.RB1 //Pump
/*#define LEDR LATAbits.LATA0 //Led Red
#define LEDG LATAbits.LATA1 //LED green*/

void SetupIOs(void)
{
    //Internal OSC:
    OSCCONbits.IRCF = 5;  //4MHz
    
	//IO Ports:
    ANSELA = 0x00; TRISA = 0x03; // Port A digital input.
    ANSELB = 0x00; TRISB = 0x00; LATB = 0x00;  // PortB digital output.
    ANSELC = 0x00; TRISC = 0x00; LATC = 0x00;  //PortC digital input.
    ANSELE = 0x00; TRISE = 0x01; //PortE digital input.
    
    //Timer0:
    T0CON = 0b10010011;
    INTCONbits.TMR0IE = 1;
    //ADC:
    ADCON0bits.ADON = 1;
    ADCON2 = 0b00001001; 
    //PORTB
    INTCON2bits.RBPU = 0;  // Enable PORTB pull ups.
    //INTCON2bits.INTEDG0 = 0;   //INT0 reacts to a falling edge. (-ve)  
    
    //Global interrupt:
    INTCONbits.GIE = 1;
}   
 
void main(void)
{
    unsigned int LocalTicks_S=0, LocalTicks_1=0, LocalTicks_2=0;
    unsigned char chan_sel=0;

    SetupIOs();   //System setup

    //Power up delay:
    Delay10KTCYx(25); // 250 ms delay @ Fosc = 4 MHz
    //Delay2=Pump_Delay+10;
    //LCD:
    InitLCD(); //initialize LCD
    
    DispRomStr(Ln1Ch0, (rom *)"Automatic hand gel dispenser");
    DispRomStr(Ln2Ch0, (rom *)"Welcome! Place Hand");
    Delay10KTCYx(100); //Debounce delay  
    InitLCD(); //Initialize LCD
    
    while(1)  //main infinite loop
    {
        if(SystemTick)
        {
            SystemTick = 0;    //clear software flag
            
            if (++LocalTicks_1 == CUSTOM_TICKS_1)   //match?, process 1 @50ms
            {
                LocalTicks_1 = 0;    //clear
                /////////////////////////////////
                //TODO on every CUSTOM_TICKS_1:
                
                ////////////////////////////////////////////////////////////////
                //If of first flag, increment button.
                if(F1 == 0) //initialize Flag 1
                {
                    if(B1 == 0) //If B1 is pressed.  
                        {
                          PM1 = 1; //Turn on Pump.
                          Pump_Countdown = Pump_Delay; //Initialize counter
                          F1=1;  //set flag
                          InitLCD(); //Initialize LCD
                          DispRomStr(Ln2Ch0, (rom *)"Please keep hand");
                          Delay10KTCYx(100); //Debounce delay  
                          InitLCD(); //Initialize LCD
                          
                          while(B1==0) 
                          {
                              //Return_Val();
                              Delay10KTCYx(10); //Debounce delay  
                              PM1=0;
                              F1=0; //Added for the cycle to repeat
                          }
                        
                        }
                    ////////////////////////////////////////////////////////////
                    //else if B1
                    else 
                    {   
                        if(PM1 == 1) //if the pump is already on, turn it off.
                            {
                              PM1 = 0; //If the pump is on turn it off
                              DispRomStr(Ln2Ch0, (rom *)"Hand removed!"); //This line works if i put the flags in a comment
                              Delay10KTCYx(100); //Debounce delay
                              InitLCD(); //Initialize LCD
                              
                            }
                    }
                    
                ////////////////////////////////////////////////////////////////    
                //Second Button, with Flag F2.    
                if (F2==0) //Initialize Flag 2
                {     
                    if(B2 == 0) //if B2 is pressed  
                    {
                    //TODO when released:
                    Pump_Delay = Pump_Delay + 10; //When B2 is pressed, Pump delay increments.
                    //Pump_Delay=Pump_Countdown;
                    //Delay10KTCYx(100); //debounce delay of 20 ms
                    F2=1;
                    
                    while(B2 == 0) //While B3 is still pressed
                          {
                            Delay10KTCYx(10); //Debounce delay  
                            //char Inc_Count(2);
                            DispRomStr(Ln1Ch0, (rom *)"++");
                            DispRomStr(Ln2Ch0, (rom *)"25 ml were added!");
                            //LEDR =1;
                            //LATAbits.LATA0 = 1;
                            Delay10KTCYx(100); //Debounce delay 
                            InitLCD();
                            F2=0;
                          }
                    }    
                }
                    else
                    {
                       if(B2==1)
                       {
                           //DispRomStr(Ln1Ch0, (rom *)"Automatic hand gel dispenser");
                           //DispRomStr(Ln2Ch0, (rom *)"Welcome! Place Hand");
                           InitLCD(); //Initialize LCD
                           F2=1;
                       }
                    }    
                
                ////////////////////////////////////////////////////////////////
                //If third button, decrement button
                if (F3==0) //Initialize Flag 2
                {     
                    if(B3 == 0) //if B2 is pressed  
                    {
                    //TODO when released:
                    Pump_Delay = Pump_Delay - 10; //When B2 is pressed, Pump delay increments.
                   
                    //Delay10KTCYx(100); //debounce delay of 20 ms
                    F3=1;
                    
                    while(B3 == 0) //While B3 is still pressed
                          {
                            Delay10KTCYx(10); //Debounce delay  
                            //char Dec_Count(2);
                            DispRomStr(Ln1Ch0, (rom *)"--");
                            DispRomStr(Ln2Ch0, (rom *)"25 ml were Removed");
                            
                            Delay10KTCYx(100); //Debounce delay 
                            InitLCD();
                            F3=0;
                          }
                    }    
                }
                    else
                    {
                       if(B3==1)
                       {
                           DispRomStr(Ln1Ch0, (rom *)"Automatic hand gel dispenser");
                           DispRomStr(Ln2Ch0, (rom *)"Welcome! Place Hand");
                           InitLCD(); //Initialize LCD
                           F3=1;
                       }
                    }
                   
                }
                ////////////////////////////////////////////////////////////////    
                else //else taba3 if l flag F1
                {   
                   if(B1==0) //If S=0 in other words
                  { 
                    PM1=0; //Turn off pump, red light
                    F1=0; //Clear flag.
                  }
                }
            
            }     
            ///////////////////////////////////////////////////////////////////
            //If taba3 second process
            if (++LocalTicks_2 == CUSTOM_TICKS_2)   //match? process 2 @500ms
            {
                LocalTicks_2 = 0;    //clear
                /////////////////////////////////
                //TODO on every CUSTOM_TICKS_2:
                if(Pump_Countdown>0)
                {
                  Pump_Countdown--;
                }
                else
                {
                  PM1=0; //Turn off pump.
                }
            }
       }
 }
}
//ISR function
#pragma code ISR = 0x0008
#pragma interrupt ISR

void ISR(void)
{
    if (INTCONbits.T0IF)   //Timer 0?
    {
        ServeTimer0();   // Service Timer0
        INTCONbits.T0IF = 0;   //clear flag
    }
}

//Timer 0
void ServeTimer0(void)
{
    //Reload timer 0 for 50ms @ Fosc = 4MHz
    TMR0H = (0xFFFF - 3125) >> 8;
    TMR0L = (0xFFFF - 3125) & 0xFF;

    //TODO:
    SystemTick=1;  //Flag
}

/*
void ServeTimer1(void)
{
    //Reload timer 0 for 50ms @ Fosc = 4MHz
    TMR0H = (0xFFFF - 3125) >> 8;
    TMR0L = (0xFFFF - 3125) & 0xFF;

    //TODO:
    SystemTick=1;  //Flag
}
*/

/*
char Return_Val()
{
    Pump_Delay = Pump_Countdown;
    
    if(B2==0)
    {
        inc_val = Pump_Delay + 1;
    }
    return inc_val;

    if(B3 ==  0)
    {
        dec_val = Pump_Delay - 1;
    }
    return dec_val;
}


char Inc_Count(int count)
{
    int count=0;
    B2.count= count;
    for(count=0; count <=8 ;count++)
    {
        if(B2.count >= 9)
        {
           Pump_Delay++;
           DispRomStr(Ln1Ch0, (rom *)"Max reached!");
        }
        else
        {
            Pump_Delay = Pump_Countdown;
        }
    }
    return Pump_Delay;
}

char Dec_Count(int count1)
{
    int count=0;
    for(count=0; count <=8 ;count++)
    {
        B3.count = count1;
        
        if(B3.count1 <= 1)
        {
           Pump_Delay--;
           DispRomStr(Ln1Ch0, (rom *)"Max reached!");
        }
        else
        {
            Pump_Delay = Pump_Countdown;
        }
    }
    return Pump_Delay;
}*/
