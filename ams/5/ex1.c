#include <stdio.h>
#include <stdbool.h>

//  (Task 1) Complete the declaration of a new struct type called screen_pos_t.
//  This structure could be used to record a position in a two-dimensional
//  discrete coordinate system.
//
//  Members:
//      x_pos - an int which records the horizontal position of the
//                     point.
//      y_pos - an int which records the vertical position of the
//                     point.

typedef struct screen_pos_t
{
    //  (Task 1.1) Declare a field called x_pos of type int.
    int x_pos;
    //  (Task 1.2) Declare a field called y_pos of type int.
    int y_pos;
} screen_pos_t;

//  (Task 2) Define a function called screen_pos_read which uses printf and
//  scanf to get the user to enter a screen_pos_t coordinate pair.
//
//  Parameters:
//      scr_pos - the address of a screen_pos_t which must be populated by the
//      function call.
//
//  Returns:
//      The function must return a boolean value indicating the status of the
//      I/O operation. The status is true if and only if two integer values
//      have been successfully parsed and saved in scr_pos.

bool screen_pos_read ( screen_pos_t * scr_pos )
{
    //  (Task 2.1) Declare an integer called items_read with initial value 0.
    int items_read = 0;
    //  (Task 2.2) Display prompt "Please enter x_pos: ".
    printf("Please enter x_pos: ");
    //  (Task 2.3) Use scanf to try to read an integer value using scanf into
    //  the scr_pos->x_pos field of the struct pointed at by scr_pos. Add
    //  the value returned by scanf to items_read.
    items_read += scanf("%d", &scr_pos->x_pos);
    //  (Task 2.4) Display prompt "Please enter y_pos: ".
    printf("Please enter y_pos: ");
    //  (Task 2.5) Use scanf to try to read an integer value into the y_pos
    //  field of the struct pointed at by scr_pos.
    //  Add the value returned by scanf to items_read.
    items_read += scanf("%d", &scr_pos->y_pos);
    //  (Task 2.6) Return true if and only if items_read is equal to 2.
    if (items_read == 2) {
      return true;
    }
}

//  (Task 3) Define a function called screen_pos_write which uses printf to
//  display the value of a screen_pos_t structure.
//
//  Parameters:
//      scr_pos - a screen_pos_t structure that will be displayed.
//
//  Returns:
//      Nothing.

void screen_pos_write ( screen_pos_t scr_pos )
{
    //  (Task 3.1) Print the horizontal and vertical position of SCREEN-POS_VAR
    //  with format string "{%d, %d}". Do NOT insert a linefeed.
    printf("{%d, %d}", scr_pos.x_pos, scr_pos.y_pos);
}

//  (Task 4) Define a function called screen_pos_dist which calculates
//  the city block distance between two screen_pos_t objects.
//  Ref: https://en.wikipedia.org/wiki/Taxicab_geometry
//
//  Parameters:
//      scr_pos_1 - a screen_pos_t structure.
//      scr_pos_2 - a screen_pos_t structure.
//
//  Returns:
//      An int which is equal to the city block distance between the points
//      represented by the arguments.

int screen_pos_dist ( screen_pos_t scr_pos_1, screen_pos_t scr_pos_2 )
{
    //  (Task 4.1) Compute and return the city block distance between the points
    //  represented by scr_pos_1 and scr_pos_2.
    int x1 = scr_pos_1.x_pos;
    int x2 = scr_pos_2.x_pos;
    int y1 = scr_pos_1.y_pos;
    int y2 = scr_pos_2.y_pos;

    int dx = x1 - x2;
    int dy = y1 - y2;

    if (dx < 0) {
      dx *= -1;
    }
    if (dy < 0) {
      dy *= -1;
    }

    int distance = dx + dy;

    return distance;
}

//  (Task 5) Define a function called screen_pos_nearest which finds the
//  screen_pos_t object in a list which is closest to a query point, using
//  city block distance.
//
//  Parameters:
//      query_pos - a screen_pos_t structure.
//      positions - an array of screen_pos_t structures.
//      num_positions - an int which tells the function how many elements there
//      are in the array.
//
//  Returns:
//      A pointer to a screen_pos_t object. If num_positions is equal to or less
//      than 0 this value will be NULL. Otherwise, it will be the address
//      of the screen_pos_t object in the array that is nearest to the query
//      point. If multiple candidates share the minimum distance to the query
//      point, return the address of the first object in the list for which
//      this is so.

screen_pos_t * screen_pos_nearest (screen_pos_t query_pos, screen_pos_t positions[], int num_positions)
{
    //  (Task 5.1) If num_positions equal to or less than 0, return NULL.
    if (num_positions <= 0) {
      return NULL;
    }

    //  (Task 5.2) Declare and initialise a pointer to screen_pos_t called nearest.
    //  The initial value is the address of the first element in the array.
    screen_pos_t *nearest = &positions[0];
    //  (Task 5.3) Declare and initialise an integer called min_dist.
    //  The initial value is the city block distance from the query to
    //  the first element of the array.
    //  Hint: use screen_pos_dist.
    int min_dist;

    min_dist = screen_pos_dist(query_pos, positions[0]);
    //  (Task 5.4) Set up a for loop to iterate over the array.
    //  Skip the first element of the array, because we already know
    //  the distance from the first element to the query.
    for (size_t i = 1; i < num_positions; i++) {
      //  (Task 5.4.1) Compute the city block distance from the query
      //  to the current element of the array. This is the current
      //  distance. Make sure you remember it somehow.
      int current_dist = screen_pos_dist(query_pos, positions[i]);

      //  (Task 5.4.2) If the current distance is less than min_dist:
      if (current_dist < min_dist) {
        //  (Task 5.4.3) Overwrite min_dist with the current distance.
        //  Overwrite nearest with the address of the current element of
        //  the array.
        min_dist = current_dist;
        nearest = &positions[i];
      }
    }

    //  (Task 5.5) Return nearest.
    return nearest;
}


#define MAX_ITEMS (100)

int main(void)
{
    screen_pos_t query;
    printf("Input query point:\n");
    screen_pos_read(&query);

    screen_pos_t ref_points[MAX_ITEMS] = { {0,0} };
    int num_items;

    // Get number of ref_points.
    printf("Please enter number of items (up to %d) that will be processed: ", MAX_ITEMS);
    scanf("%d", &num_items);

    // if number of ref_points exceeds array size, restrict it to that value.
    if (num_items > MAX_ITEMS)
    {
        num_items = MAX_ITEMS;
    }

    for (int i = 0; i < num_items; i++)
    {
        printf("Please enter item %d of %d:\n", (i + 1), num_items);
        screen_pos_read(&ref_points[i]);
    }

    for (int i = 0; i < num_items; i++)
    {
        printf("Distance from ");
        screen_pos_write(ref_points[i]);
        printf(" to ");
        screen_pos_write(query);
        printf(" is %d\n", screen_pos_dist(ref_points[i], query));
    }

    screen_pos_t * nearest = screen_pos_nearest(query, ref_points, num_items);

    if (nearest)
    {
        printf("The closest point is ");
        screen_pos_write(*nearest);
        printf(", at a distance of %d.\n", screen_pos_dist(query, *nearest));
    }
    else
    {
        printf("Collection is empty.\n");
    }

    return 0;
}
