#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>


// Add additional constants here

// Variables used to store the game’s dynamic state.
bool game_over = false; /* Set this to true when game is over */
bool update_screen = true; /* Set to false to prevent screen update. */
bool paused = false;

//timer variables
double start_time;
double current_time = 0;

char * hero_image =
/**/"  ()  "
/**/"--[]--"
/**/"  /\\  ";



sprite_id hero;
// Define additional global variables here.
int score = 0;
int lives = 10;
int w;
int h;
// int random;
// Setup game

void draw_display() {
  // Find the width, height and centre of screen
  w = screen_width();
  //int h = screen_height();
  draw_line(0, 0, w, 0, '~');
  draw_line(0, 4, w, 4, '~');
  //display titles
  draw_formatted(3, 2, "Lives: " );
  draw_formatted(15, 2, "Time: %6.2f", current_time);
  draw_formatted(22, 2, "Score: " );
  // draw student number
  draw_string(w - 10, 2, "n9966463");

}
//draw blocks
// void random_number(){
//   random = 0;
//   for(size_t i = 0; i < ; i++)
//   {
//     /* code */
//   }
  
// }
// return int columns(void){
//   int max_width = 11;
//   int number_cols = (int) w/column_width;
// }
// int rows(void){
//    int row_height = 5;
//   return int number_rows = (int) (h-10)/row_height;
// }


//Set up
void setup( void) {
  start_time = get_current_time();
  draw_display();
  draw_all();
  hero = sprite_create(10, 10, 6, 3, hero_image);
  sprite_draw(hero);

  
}
// Play one turn of game.
void process( void ) {
  //timer update
  current_time = get_current_time() - start_time;

  //get charactor code from standard input
  char key = get_char();

  // screen coordinates of hero
  int x = round(sprite_x(hero));
  //int y = round(sprite_y(hero));
  //Move hero left
  
  if (key == 'a' && x > 0) {
    sprite_move(hero, -1, 0);
  }
  
  if (key == 'd' && x <(w-6)) {
    sprite_move(hero, 1, 0);
  }

  sprite_draw(hero);
  draw_display();
  clear_screen();
}


// Clean up at end of game.
void cleanup( void ) {
	// Insert statements to release resources such as files, memory.
}

// Program entry point.
int main( void ) {
	setup_screen();  // Call ZDK function to prepare screen.
	setup();         // Call our setup function to initialise game state.
	show_screen();   // Display the screen at the start of play.
	// Following block will repeat until game_over becomes true.
while ( !game_over ) {
		process();

		if ( update_screen ) {
			show_screen();
		}

	}
	// Game over is now true, so clean up and exit.
	cleanup();

	return 0;
}
