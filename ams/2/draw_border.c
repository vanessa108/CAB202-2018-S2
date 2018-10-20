#include <cab202_graphics.h>


void draw_border( void ) {
  int w = screen_width();
  int h = screen_height();
	// (a) Declare an integer variable called left, with an initial value of 5.
  int left = 5;
	// (b) Declare an integer variable called top, with an initial value of 4.
  int top = 4;
	// (c) Declare an integer variable called right, with an initial value 5
	//     less than the largest visible horizontal location in the terminal window.
  int right = w - 1 - 5;
	// (d) Declare an integer variable called bottom, with an initial value 1
	//     less than the maximum visible vertical location in the terminal window.
  int bottom = h - 1 -1;
    // (e) Draw a line from (left, top) to (right, top), using the '!' symbol.
    draw_line( left, top, right, top, '!');
    // (f) Draw a line from (right, top) to (right, bottom), using the '!' symbol.
    draw_line( right, top, right, bottom, '!');
    // (g) Draw a line from (left, bottom) to (right, bottom), using the '!' symbol.
    draw_line( left, bottom, right, bottom, '!');
    // (h) Draw a line from (left, top) to (left, bottom), using the '!' symbol.
    draw_line( left, top, left, bottom, '!');
    // Do not touch the following line.
    show_screen();
}

int main( void ) {
	setup_screen();
	draw_border();
	wait_char();
	cleanup_screen();
	return 0;
}
