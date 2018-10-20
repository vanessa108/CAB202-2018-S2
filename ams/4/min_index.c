#include <math.h>
#include <stdbool.h>
#include <stdio.h>

//  (a) Begin the definition of a function called min_index
//  which locates the minimum value in an array and returns the
//  smallest index of the minimum value in the array.
//
//  Parameters:
//      values   - an array of double precision floating point values.
//      num_vals - an int which specifies the number of items to process.
//
//  Returns:
//      An int indicating the location at which the minimum may be found:
//      *   If num_vals is 0: return -1 to record the fact that there is
//          no data to process.
//      *   Otherwise: return i in 0 .. (num_vals-1) such that:
//          (For all j in 0..(num_vals-1), values[i] <= values[j]) AND
//          (For any j where values[j] == values[i], j >= i)
//      i.e.
//          values[i] is the "smallest" value in the array, and i is
//          the "smallest" index where that happens.

 int min_index(double values[], int num_vals)
{
    //  (b) If number of usable items is less than or equal to 0
    //  return -1.
    if (num_vals <= 0) {
      return -1;
    }
    //  (c) Declare variable store index position, initially 0
    int sip = 0;
    //  (d) Iterate over array, counting from 1
    for (size_t i = 0; i < num_vals; i++) {
      if (values[i] < values[sip]) {
        sip = i;
      }
    }
        //  (e) If item at current position less than item at index
        //  position, replace index position with current position.

    //  (f) Return index position.
    return sip;
}


#define MAX_ITEMS (100)

int main(void)
{
    double items[MAX_ITEMS];
    int array_size;

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

    int result = min_index(items, array_size);

    printf("The index of the minimum is %d.\n", result);

    return 0;
}
