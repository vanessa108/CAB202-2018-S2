#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>

#include <graphics.h>
#include <macros.h>
#include "lcd_model.h"

void setup(void) {
	set_clock_speed(CPU_8MHz);
	lcd_init(LCD_DEFAULT_CONTRAST);
	lcd_clear();

	//	(a) Initialise Timer 0 in normal mode so that it overflows 
	//	with a period of approximately 0.008 seconds.
	//	Hint: use the table you completed in a previous exercise.
    TCCR0A = 0;
    TCCR0B = 0x04;
	//	(b) Enable timer overflow interrupt for Timer 0.
    TIMSK0 = 1;
	//	(c) Turn on interrupts.
    sei();
	//	(d) Enable the joystick centre switch for digital input.
    CLEAR_BIT(DDRB, 0);
	//	(e) Display your student number, "n9966463", with nominal
	//	top-left corner at screen location (1,27).
    draw_string(1, 27, "n9966463", FG_COLOUR);
	// Keep the next instruction intact.
	show_screen();
}

//	(f) Create a volatile global variable called switch_counter.
//	The variable should be an 8-bit unsigned integer. 
//	Initialise the variable to 0.
volatile uint8_t switch_counter = 0;
//	(g) Define a volatile global 8-bit unsigned global variable 
//	called switch_closed which will store the current state of the switch.
volatile uint8_t switch_closed;
//	(h) Define an interrupt service routine to process timer overflow
//	interrupts for Timer 0. Every time the interrupt service
//	routine is called, switch_counter should:
//		(h.a) Left-shift switch_counter one place;
//		(h.b) Bitwise AND with a mask in which the 5 bits on the right
//			are 1 and the others are 0.
//		(h.c) Use bitwise OR to add the current open/closed value of the 
//			joystick centre switch to the history.
//		(h.d) If switch_counter is equal to the bit mask, then the switch has been 
//			observed 5 times in a row to be closed. Assign the value 1 to 
//			switch_closed, indicating that the switch should now be considered to be
//			officially "closed".
//		(h.e) If switch_counter is equal to 0, then the switch has been observed 
//			to be open at least 5 in a row, so store 0 in switch_closed, 
//			indicating that the switch should now be considered to be officially "closed".
ISR(TIMER0_OVF_vect) {
    switch_counter = ((switch_counter << 1) & 0b00011111) | BIT_IS_SET(PINB, 0);
    if (switch_counter == 0b00011111) {
        switch_closed = 1;
    } else if (switch_counter == 0) {
        switch_closed = 0;
    }
}

// -------------------------------------------------
// Test driver.
// -------------------------------------------------
char count[14];
int ctr;
void process(void) {
	static uint8_t prevState = 0;
	//static uint8_t currentState = 100;
	
	if ( switch_closed != prevState ) {
		prevState = switch_closed;
		ctr++;
		draw_string( 30, 40, prevState ? "closed" : "open  ", FG_COLOUR);
		sprintf(count, "%d", ctr);
		draw_string( 5, 5, count, FG_COLOUR);
		show_screen();
	}
}

int main(void) {
	setup();

	for ( ;; ) {
		process();
	}
}
