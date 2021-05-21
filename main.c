# define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include "SSD1306.h"

// Declare Global Variables
volatile int rpm;
volatile int counter;
volatile int RPMcounter;

int main (void) {

//Timer 4 setup
TCNT4H = 78;   //Value for 1 second delay
TCNT4L = 12;
TCCR4A = 0x00;
TCCR4B = 0x05;// 1024 Prescaler, normal mode
TIMSK4 = (1<<TOIE4);

uint8_t dispNum = 0x00;
char text[] = "TEMPLE MADE ECE";
OLED_Init();  //initialize the OLED
_delay_ms(1);
OLED_Clear(); //clear the display (for good measure)

PORTB = 0xFF;
counter = 0;// Initialize both counters
RPMcounter = 0;
DDRA = 0xFF;//Port A has 8 Leds as outputs
DDRD = 0;// Port D all inputs
PORTD |= (1<<3);// Set Pull up resistor PD2 (INT1)

EIMSK=(1<<INT1);
EICRA=0X0C; //INT0 WILL ACTIVATE Rising EDGE TRIGGER

sei();// Set Interrupt Flag

OLED_SetCursor(0, 0);        //set the cursor position to (0, 0)
OLED_Printf("GO OWLS   "); //Print out some text
while(1){
// Wait For Interrupts and Update LEDs
PORTA = counter;
OLED_SetCursor(4, 0);
OLED_Printf(" uint8_t dispNum: ");
OLED_DisplayNumber(C_DECIMAL_U8,counter,3);

OLED_SetCursor(6, 0);
OLED_Printf(" RPM: ");
OLED_DisplayNumber(C_DECIMAL_U8,rpm,3);

OLED_HorizontalGraph(2,counter*100/256);
clockwise();
}
}

void clockwise(){
DDRD = 1<<2;
// 1.3msec pulse
TCNT1H = 0xAE;
TCNT1L = 0xBF;
TCCR1A = 0x00;
TCCR1B = 0x01;
PORTD = 0b0000100;
while((TIFR1&(1<<TOV1))==0);
TCCR1A = 0;
TCCR1B = 0;
TIFR1 = (1<< TOV1);
// 20msec pulse
TCNT1H = 0xEC;
TCNT1L = 0x77;
TCCR1A = 0x00;
TCCR1B = 0x03;
PORTD = 0b0000000;
while((TIFR1&(1<<TOV1))==0);
TCCR1A = 0;
TCCR1B = 0;
TIFR1 = (1<< TOV1);
}

// External Interrupt on Port D Pin 3
ISR(INT1_vect, ISR_BLOCK)
{
counter++; // Increment both counters when optical switch changes
RPMcounter++;
}

ISR (TIMER4_OVF_vect) //ISR for Timer4 overflow. Happens every 1 seconds
{
rpm = RPMcounter*(60); //Find RPM counter after 1 sec, multiply by 60 to get rpm
TCNT4H = 78;  
TCNT4L = 12;    
RPMcounter = 0; //Reset counter
}
