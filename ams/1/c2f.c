#include <stdio.h>

void c2f(void) {
	// Insert your solution here.
  // (a) Prompt user to enter value.
  printf("Please enter the temperature in Celsius degrees: ");
  // (b) Declare local variable to hold input value.
  double fahrenheit, celsius;
  // (c) Read input value.
  scanf("%lf", &celsius );
  // (d) Compute output value.
  fahrenheit = ((9.0/5.0)*celsius) + 32;
  // (e) Display the result.
  printf("A temperature of %.4f degrees Celsius is equal to %.4f degrees Fahrenheit.\n", celsius, fahrenheit);
}


int main(void) {
	c2f();
	return 0;
}
