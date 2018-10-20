/*
**	usb_game_controller.c
**
**	A simple TeensyPewPew app which sends encoded joystick
**	signals to desktop via USB.
**
**	Lawrence Buckingham, QUT, October 2017.
**	(C) Queensland University of Technology.
*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>
#include <macros.h>
#include <graphics.h>
#include <sprite.h>

#include "usb_serial.h"

void setup(void);
void setup_joystick(void);
void setup_usb_serial(void);
void usb_serial_send(char * message);
void process(void);

//-------------------------------------------------------------------

int main(void) {
	setup();

	for ( ;; ) {
		process();
	}
}
//-------------------------------------------------------------------

void process(void) {
	clear_screen();
	draw_string(0, 0, "USB controller", FG_COLOUR);

	int key_code = -1;

	if ( BIT_IS_SET(PIND, 1) ) key_code = 'w';
	if ( BIT_IS_SET(PINB, 7) ) key_code = 's';
	if ( BIT_IS_SET(PINB, 1) ) key_code = 'a';
	if ( BIT_IS_SET(PIND, 0) ) key_code = 'd';
	if ( BIT_IS_SET(PINF, 5) ) key_code = 'q';

	if ( key_code > 0 ) {
		draw_string(0, 10, "TX:", FG_COLOUR);
		draw_char(3 * 5, 10, key_code, FG_COLOUR);
		usb_serial_putchar(key_code);
	}

	show_screen();
}

//-------------------------------------------------------------------

void setup(void) {
	set_clock_speed(CPU_8MHz);
	lcd_init(LCD_DEFAULT_CONTRAST);
	setup_usb_serial();
	setup_joystick();
}

//-------------------------------------------------------------------

void setup_joystick(void) {
	CLEAR_BIT(DDRD, 1);
	CLEAR_BIT(DDRB, 7);
	CLEAR_BIT(DDRB, 1);
	CLEAR_BIT(DDRD, 0);
	CLEAR_BIT(DDRF, 5);
}

// ---------------------------------------------------------
//	USB serial business.
// ---------------------------------------------------------

void setup_usb_serial(void) {
	// Set up LCD and display message
	lcd_init(LCD_DEFAULT_CONTRAST);
	draw_string(10, 10, "Connect USB...", FG_COLOUR);
	show_screen();

	usb_init();

	while ( !usb_configured() ) {
		// Block until USB is ready.
	}

	clear_screen();
	draw_string(10, 10, "USB connected", FG_COLOUR);
	show_screen();
}

/*
**	Transmits a string via usb_serial.
*/
void usb_serial_send(char * message) {
	// Cast to avoid "error: pointer targets in passing argument 1 
	//	of 'usb_serial_write' differ in signedness"
	usb_serial_write((uint8_t *) message, strlen(message));
}

//-------------------------------------------------------------------
