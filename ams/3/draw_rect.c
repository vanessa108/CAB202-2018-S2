#include <stdlib.h>
#include <cab202_graphics.h>

// (a) Begin the definition a function called draw_rect that returns nothing,
//      but requires the following arguments:
//      leftmost - an integer that specifies the absolute horizontal location
//          of the left hand side of the rectangle.
//      upper - an integer that specifies the absolute vertical location
//          of the upper edge of the rectangle.
//      rightmost - an integer that specifies the absolute horizontal location
//          of the right hand side of the rectangle.
//      lower - an integer that specifies the absolute vertical location
//          of the lower edge of the rectangle.
//      display_char = a character value that will be used to draw the
//          display the rectangle.

 void draw_rect (int leftmost, int upper, int rightmost, int lower, char display_char)
{
	//  (b) Test to see if either the width or height of the rectangle is less
	//      than 1. If so, the function should stop immediately and draw nothing.
  int width = rightmost - leftmost;
  int height = lower - upper;
  if (width < 0 || height < 0) {
    return;
  }
	//  (c) Draw a horizontal line from left to right at the top, using the
	//      display char.
  draw_line(leftmost, upper, rightmost, upper, display_char);
	//  (d) Draw a horizontal line from left to right at the bottom, using the
	//      display char.
  draw_line(leftmost, lower, rightmost, lower, display_char);
	//  (e) Draw a horizontal line from top to bottom at the left, using the
	//      display char.
  draw_line(leftmost, upper, leftmost, lower, display_char);
	//  (f) Draw a horizontal line from top to bottom at the right, using the
	//      display char.
  draw_line(rightmost, upper, rightmost, lower, display_char);
}


int main( void ) {
	int l, t, r, b;
	char c;

	printf( "Please enter the horizontal location of the left edge of the rectangle: " );
	scanf( "%d", &l );

	printf( "Please enter the vertical location of the top edge of the rectangle: " );
	scanf( "%d", &t );

	printf( "Please enter the horizontal location of the right edge of the rectangle: " );
	scanf( "%d", &r );

	printf( "Please enter the vertical location of the bottom edge of the rectangle: " );
	scanf( "%d", &b );

	printf( "Please enter the character used to draw the rectangle? " );
	scanf( " %c", &c );

	setup_screen();
	draw_rect( l, t, r, b, c );
	show_screen();
	wait_char();

	return 0;
}
