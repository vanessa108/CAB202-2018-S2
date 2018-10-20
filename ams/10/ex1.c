#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>

#include <graphics.h>
#include <macros.h>
#include "lcd_model.h"



void setup_uart(void) {
    //	(a) Implement a function called setup_uart which requires no arguments and
//	returns no value.
	//	(a.a) Set the data transmission rate to 19200 bits per second,
	//	using the formula provided in the Topic 10 Technical Notes.
    uint32_t baud = 19200;
    UBRR1 = (F_CPU / 4 / baud - 1) / 2;
	//	(a.b) Set the UART speed to double speed.
    SET_BIT(UCSR1A, 1);
	//	(a.c) Enable receive and transmit, but do not enable any UART-related
	//	interrupts.
    SET_BIT(UCSR1B, 4);
	SET_BIT(UCSR1B, 3);
	//	(a.d) Set the character size to 8 bits, with no parity
	//	and 1 stop bit.
    //8bits
    SET_BIT(UCSR1C, 1);
    SET_BIT(UCSR1C, 2);
    CLEAR_BIT(UCSR1B, 2);
    CLEAR_BIT(UCSR1C, 4); //parity
    CLEAR_BIT(UCSR1C, 5); // parity
    CLEAR_BIT(UCSR1C, 3); //STOP
//	(b) End of function setup_uart
}


int main(void) {
	setup_uart();

	for ( ;; ) {}
}
