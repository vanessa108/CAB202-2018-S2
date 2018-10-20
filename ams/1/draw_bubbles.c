#include "cab202_graphics.h"

void draw_bubbles(void){
	// (a) Draw a bubble at (21,12)
  draw_char(21, 12, 'O');
	// (b) Draw a bubble at (51,2)
  draw_char(51, 2, 'O');
	// (c) Draw a bubble at (50,10)
  draw_char(50, 10, 'O');
	// (d) Draw a bubble at (47,0)
  draw_char(47, 0, 'O');
	// Keep the following line without change.
	show_screen();
}

int main( void ) {
	setup_screen();

	draw_bubbles();

	draw_string( 0, screen_height() - 1, "Press any key to finish..." );
	wait_char();
	cleanup_screen();
	return 0;
}
