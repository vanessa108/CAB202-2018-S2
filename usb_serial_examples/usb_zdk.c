/*
**	usb_zdk.c
**
**	Control a sprite in ZDK using chars from a file, assumed to be
**	a USB serial device.
**
**	Lawrence Buckingham, QUT, October 2017.
**	(C) Queensland University of Technology.
*/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

void setup(const char * serial_device);
void setup_hero(void);
void setup_usb_serial(const char * serial_device);
void process(void);

FILE *usb_serial;
bool loop = true;

//-------------------------------------------------------------------

int main(int argc, char *argv[]) {
	if ( argc != 2 ) {
		fprintf(stderr, "Expected 1 command line argument containing serial device name.\n");
		fprintf(stderr, "Example: usb_zdk /dev/ttyS3\n");
		return 1;
	}

	setup(argv[1]);

	while(loop) {
		process();
	}
	
	
	return 0;
}
//-------------------------------------------------------------------

Sprite hero;

void process(void) {
	int dx = 0, dy = 0;

	int c = fgetc(usb_serial);

	if ( c == 'a' ) dx--;
	if ( c == 'd' ) dx++;
	if ( c == 's' ) dy++;
	if ( c == 'w' ) dy--;
	if ( c == 'q' ) loop = false;

	if ( dx || dy ) {
		hero.x += dx;
		hero.y += dy;
		clear_screen();
		sprite_draw(&hero);
		show_screen();
	}
}

//-------------------------------------------------------------------

void setup(const char * serial_device) {
	setup_screen();
	setup_usb_serial(serial_device);
	setup_hero();
}

//-------------------------------------------------------------------

void setup_hero(void) {
	static char bitmap[] = {
		"  ****  "
		" *    * "
		"* *  * *"
		"*      *"
		"* *  * *"
		"*  **  *"
		" *    * "
		"  ****  "
	};
	int W = 8, H = 8;
	sprite_init(&hero, (screen_width() - W) / 2, (screen_height() - H) / 2, W, H, bitmap);
	clear_screen();
	sprite_draw(&hero);
	show_screen();
}

// ---------------------------------------------------------
//	USB serial business.
// ---------------------------------------------------------

void setup_usb_serial(const char * serial_device) {
	usb_serial = fopen(serial_device, "r+");

	if ( usb_serial == NULL ) {
		fprintf(stderr, "Unable to open device \"%s\"\n", serial_device);
		exit(1);
	}
}

//-------------------------------------------------------------------
