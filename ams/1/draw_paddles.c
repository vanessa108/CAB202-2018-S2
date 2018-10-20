#include "cab202_graphics.h"
void draw_paddles( void ) {
	// INSERT CODE HERE
  int w = screen_width();
  int h = screen_height();
  int right_side = w - 1;
  int ytl = (h - 5) / 2  + 2;
  int ybl = ytl + 5 -1;
  int ytr = (h - 5) / 2 - 4;
  int ybr = ytr + 5 -1;

  draw_line(3, ytl, 3, ybl, '^');
  draw_line(right_side - 4, ytr, right_side - 4, ybr, '^');
	show_screen( );
}


int main( void ) {
	setup_screen();
	draw_paddles();
	draw_string( 0, screen_height( ) - 1, "Press any key to finish..." );
	wait_char();
	cleanup_screen();
	return 0;
}
