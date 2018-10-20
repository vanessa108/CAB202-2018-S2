#include "cab202_graphics.h"

void draw_platform(void) {
	// (a) Get the horizontal position of the left end of the platform
	//     using integer arithmetic and storing it in an integer value.
  int W = screen_width();
  int L = 14;
  int left_edge = (W - L) / 2;
	// (b) Get the vertical position of the platform.
  int H = screen_height();
  int vertical_position = H-1-3;
	// (c) Draw a horizontal line 14 units long starting at the position
	//     you have calculated, using '_' symbols.
  draw_line(left_edge, vertical_position, left_edge + 13, vertical_position, '_');
	// Leave the following line intact.
	show_screen();
}


int main( void ) {
	setup_screen( );

	draw_platform( );

	draw_string( 0, screen_height( ) - 1, "Press any key to finish..." );
	wait_char( );
	cleanup_screen( );
	return 0;
}
