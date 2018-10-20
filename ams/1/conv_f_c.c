#include <stdio.h>

void conv_f_c(void) {
	// (a) Prompt user to enter value.
  printf("Please enter the temperature, measured in degrees Fahrenheit: ");
	// (b) Declare local variable to hold input value.
  double fahrenheit, celsius;
	// (c) Read input value.
  scanf("%lf", &fahrenheit );
	// (d) Compute output value.
  celsius = (fahrenheit - 32) * 5 / 9;
	// (e) Display the result.
  printf("A temperature of %.5f degrees Fahrenheit is equal to %.5f degrees Celsius.\n", fahrenheit, celsius);
}

int main(void) {
	conv_f_c();
	return 0;
}
