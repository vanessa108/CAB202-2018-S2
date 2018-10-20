#include <stdint.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>

#include <graphics.h>
#include <macros.h>

void setup(void) {
	set_clock_speed(CPU_8MHz);
	lcd_init(LCD_DEFAULT_CONTRAST);
	clear_screen();
	show_screen();
}

void process(void) {
	char *hello = "Hello Teensy!";
	clear_screen();
	draw_string(0, 0, hello, FG_COLOUR);
	draw_string(0, 8, hello, BG_COLOUR);
	draw_string(0, 16, hello, FG_COLOUR);
	draw_string(0, 24, hello, BG_COLOUR);
	draw_string(0, 32, hello, FG_COLOUR);
	draw_string(0, 40, hello, BG_COLOUR);
	show_screen();
}

int main(void) {
	setup();

	for ( ;; ) {
		process();
		_delay_ms(10);
	}
}