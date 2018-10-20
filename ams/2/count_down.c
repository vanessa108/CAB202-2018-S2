#include <stdio.h>

void count_down( void ) {
	// (a) Print the introductory message.
  printf("Counting backwards in steps of 7, beginning at 310, stopping before 29.\n");
	// (b) Declare an integer variable that you will use to count. Initially
	//     the counter should equal the start value, 310.
  int counter = 310;
	// (c) Begin a WHILE statement that will execute its loop body if the
	//     counter is greater than the end value, 29.
  while (counter > 29) {
    printf("%d\n", counter);
    counter = counter - 7;
  }
		// (e) Print the value of the counter on a line by itself.

		// (f) Subtract the step size, 7, from the counter.
	// (d) End the WHILE statement
}

int main( void ) {
	count_down();
	return 0;
}
