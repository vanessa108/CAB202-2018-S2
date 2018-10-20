#include <stdio.h>

//  (a) Begin the definition a function called maximum which returns
//      the maximum of its two double precision floating point arguments.
//
//      For all pairs of double precision floating point values x and y, the
//      function must satisfy the following conditions:
//          ((maximum(x,y) = x) OR (maximum(x,y) = y))
//      AND (maximum(x,y) >= x)
//      AND (maximum(x,y) >= y)

 double maximum( double x, double y )
{
    //  (b) If the value of the first argument is greater than or equal to that of the
    //      second argument: return the value of the first argument.
    if (x >= y) {
      return x;
    }
    //  (c) Otherwise: return the value of the second argument.
    else {
      return y;
    }

}

int main(void) {
    double x, y, maxVal;

    printf("Please enter two numeric values: ");
    scanf("%lf%lf", &x, &y);
    maxVal = maximum(x, y);
    printf( "maximum(%0.10f, %0.10f) = %0.10f\n", x, y, maxVal);

    return 0;
}
