#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

// Configuration
#define DELAY (10) /* Millisecond delay between game updates */

#define ZOMBIE_WIDTH (5)
#define ZOMBIE_HEIGHT (5)

// Game state.
bool game_over = false; /* Set this to true when game is over */
bool update_screen = true; /* Set to false to prevent screen update. */

char * zombie_image =
/**/	"ZZZZZ"
/**/	"   Z "
/**/	"  Z  "
/**/	" Z   "
/**/	"ZZZZZ";

//	(a) Declare a sprite_id called zombie
sprite_id zombie;
int w, x, h, y;
// Setup game.
void setup(void) {
    //  (b) Assign a value to zombie by creating a sprite at integer
    //      coordinates (x,y). Use integer arithmetic to calculate
    //      the location relative to the screen dimensions:
    //      x = 30% of available horizontal space on the screen.
    //      y = 10% of available vertical space on the screen.
    //      Use the formula set out in the statement of the problem.
    w = screen_width();
    x = (w - 5) * 30 / 100;
    h = screen_height();
    y = (h - 5) * 10 /100;
    //	(c) Draw the zombie.
    zombie = sprite_create(x, y, 5, 5, zombie_image);
    sprite_draw( zombie);
}

// Play one turn of game.
void process(void) {
    //	(d) Get a character code from standard input without waiting.
     char key = get_char();
    //  (e) Get the current screen coordinates of the zombie in integer variables
    //      by rounding the actual coordinates. Store the coordinates in a pair of
    //      variables called x and y, both of type int.
    int x = round(sprite_x(zombie));
    int y = round(sprite_y(zombie));
    //	(f) Move zombie left, ensuring that it always remains within the screen.
    if (key == '4' && x > 0) {
      sprite_move(zombie, -1, 0);
    }
    //	(g) Move zombie right, ensuring that it always remains within the screen.
    if (key == '6' && x < w - 5) {
      sprite_move(zombie, 1, 0);
    }
    //	(h) Move zombie up, ensuring that it always remains within the screen.
    if (key == '8' && y > 0) {
      sprite_move(zombie, 0, -1);
    }
    //	(i) Move zombie down according to specification.
    if (key == '2' && y < h - 5) {
      sprite_move(zombie, 0, 1);
    }
    //	(j) If the key is 'q', set game_over to true.
    if (key == 'q') {
      game_over = true;
    }
    // Leave next line intact.
    clear_screen();

    //	(k)	Draw the zombie.
    sprite_draw(zombie);
}

// Program entry point.
int main(void) {
	setup_screen();
	setup();
	show_screen();

	while ( !game_over ) {
		process();

		if ( update_screen ) {
			show_screen();
		}

		timer_pause(DELAY);
	}

	return 0;
}
