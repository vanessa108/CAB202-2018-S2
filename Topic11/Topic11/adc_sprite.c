/*
**	adc_sprite.c
**
**	Demonstrates ADC control. Moves a sprite according to the ADC value.
**
**	This program is derived from an earlier unattributed source file.
**
**	Current custodian: Lawrence Buckingham, Queensland University of
**	Technology.
*/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lcd.h"
#include "cpu_speed.h"
#include "sprite.h"
#include "graphics.h"
#include "macros.h"
#include "cab202_adc.h"

void setup(void);
void process(void);

#define THRESHOLD 512

int main() {
	setup();

	for ( ;; ) {
		process();
	}
}

Sprite diamond;
uint8_t diamond_bm[] = {
	0b00100000,
	0b01010000,
	0b10001000,
	0b01010000,
	0b00100000,
};

void setup(void) {
	set_clock_speed(CPU_8MHz);
	adc_init();
	lcd_init(LCD_DEFAULT_CONTRAST);
	sprite_init(&diamond, 0, 0, 5, 5, diamond_bm);
}

#define sprite_move_to(sprite,_x,_y) (sprite.x = (_x), sprite.y = (_y))

void process(void) {
	int left_adc = adc_read(0);
	int right_adc = adc_read(1);

	clear_screen();
	sprite_move_to(diamond,(double) left_adc * (LCD_X - diamond.width) / 1024,(double) right_adc * (LCD_Y - diamond.height) / 1024);
	sprite_draw( &diamond );
	show_screen();
}
