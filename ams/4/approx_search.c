#include <math.h>
#include <stdbool.h>
#include <stdio.h>

//  (a) Begin the definition of a function called approx_search
//  which locates values from an array which lie within a small
//  margin of error of a designated query value. Return the last
//  (greatest) index value for which this holds.
//
//  Parameters:
//      values   - an array of double precision floating point values.
//      num_vals - an int which specifies the number of items to process.
//      query    - a double precision floating point value to locate.
//      tol      - a small, strictly positive, double precision floating
//                  point margin of error for equality comparisons.
//
//  Returns:
//      An int indicating the location at which the query value may be found:
//      *   If num_vals is 0: return -1 to record the fact that there is
//          no data to process.
//      *   Otherwise: return i in 0 .. (num_vals-1) such that:
//          (fabs(values[i] - query) <= tol) AND
//          (For any j where fabs(values[j] - query) <= tol: j <= i)
//      i.e.
//          values[i] is approximately equal to query, and i is the "largest" index where
//          that happens.

 int approx_search( double values[], int num_vals, double query, double tol)
{
    //  (b) Declare variable store index position, initially -1.
    int sip = -1;
    //  (c) Iterate over array, counting from 0.
    for (size_t i = 0; i < num_vals; i++) {
      if (fabs(values[i] - query) <= tol) {
        sip = i;
      }
    }
    return sip;

        //  (d) If item at current position is within +/- error of
        //  the query, replace index with current position.


    //  (e) Return index position.
}


#define MAX_ITEMS (100)

int main(void)
{
    double items[MAX_ITEMS];
    int array_size;
    double query;
    double error;

    // Get number of items.
    printf("Please enter number of items (up to %d) that will be processed: ", MAX_ITEMS);
    scanf("%d", &array_size);

    // if number of items exceeds array size, restrict it to that value.
    if (array_size > MAX_ITEMS)
    {
        array_size = MAX_ITEMS;
    }

    for (int i = 0; i < array_size; i++)
    {
        printf("Please enter (floating point) item %d of %d: ", (i + 1), array_size);
        scanf("%lf", &items[i]);
    }

    printf("Please enter (floating point) query value: ");
    scanf("%lf", &query);

    printf("Please enter (floating point) error margin: ");
    scanf("%lf", &error);

    int result = approx_search(items, array_size, query, error);

    printf("The last approximate match to the query is at position %d.\n", result);

    return 0;
}
