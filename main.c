#include <avr/io.h>
# define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include "SSD1306.h"

// Declare Global Variables
volatile int rpm;
volatile int counter;
volatile int RPMcounter;
void clockwise();

int main (void) {
	
	//Timer 4 setup 
	TCNT4H = 0x3D;	   //Value for 2 second delay
	TCNT4L = 0x08;
	TCCR4A = 0x00;
	TCCR4B = 0x05;// 1024 Prescaler, normal mode
	TIMSK4 = (1<<TOIE4); 
	
	OLED_Init();  //initialize the OLED
	_delay_ms(1);
	OLED_Clear(); //clear the display (for good measure)

	counter = 0;// Initialize both counters
	RPMcounter = 0;
	
	PORTB = 0xFF;
	DDRA = 0xFF;//Port A has 8 Leds as outputs
	DDRD = 0;// Port D all inputs
	PORTD |= (1<<3);// Set Pull up resistor PD3 (INT1)
	
	EIMSK=(1<<INT1);
	EICRA=0X0C; //INT1 WILL ACTIVATE Rising EDGE TRIGGER
	
	sei();// Set Interrupt Flag
	
	OLED_SetCursor(0, 0);        //set the cursor position to (0, 0)
	OLED_Printf("Final Project Ellisa Booker  "); //Print out some text
	
	while(1){
		// Wait For Interrupts and Spin Motor
		clockwise();			

	}
}

void clockwise(){
  DDRD = 1<<2;
  PORTD &= ~(1<<2);

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
  PORTD = PORTD^(1<<2);
}

// External Interrupt on Port D Pin 3
ISR(INT1_vect, ISR_BLOCK)
{
	counter++; // Increment both counters when optical switch changes
	RPMcounter++; 
	PORTA = counter; // Send counter out to PORTA
	
	//Update  LCD with counter and RPM values
	OLED_SetCursor(4, 0);
	OLED_Printf(" Counter: ");
	OLED_DisplayNumber(C_DECIMAL_U8,counter,3);
	
	OLED_SetCursor(6, 0);
	OLED_Printf(" RPM: ");
	OLED_DisplayNumber(C_DECIMAL_U8,rpm,3);
}

ISR (TIMER4_OVF_vect) //ISR for Timer4 overflow. Happens every 2 seconds
{
	rpm = RPMcounter*(30); //Find RPM counter after 2 sec, multiply by 30 to get rpm   
	RPMcounter = 0; //Reset counter
}

