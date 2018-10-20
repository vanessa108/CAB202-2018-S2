#include "cab202_graphics.h"

void draw_lines(void){
	// (a) Draw a line from (34,1) to (46,15).
	draw_line(34, 1, 46, 15, '@');
	// (b) Draw a line from (46,15) to (49,22).
	draw_line(46, 15, 49, 22, '@');
	// (c) Draw a line from (49,22) to (10,22).
	draw_line(49, 22, 10, 22, '@');
	// (d) Draw a line from (10,22) to (34,1).
	draw_line(10, 22, 34, 1, '@');
	// Leave the following instruction unchanged.
	show_screen();
}

int main( void ) {
	setup_screen( );

	draw_lines();

	draw_string( 0, screen_height( ) - 1, "Press any key to finish..." );
	wait_char( );
	cleanup_screen( );
	return 0;
}
