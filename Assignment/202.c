#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

//gcc 202.c -std=gnu99 -Wall -Werror -Ic:/cab202/ZDK -Lc:/cab202/ZDK -lZDK -lncurses -o 202
//game's dynamic state
bool game_over = false; 
bool update_screen = true;
bool paused = false;

/** Constants **/
#define DELAY (10) //Millisecond delay between game updates
#define WAIT (1000)

#define HERO_WIDTH (6)
#define HERO_HEIGHT (3)

#define TREASURE_WIDTH (5)
#define TREASURE_HEIGHT (2)
#define TREASURE_SPEED (0.1)

#define GRAVITY (0.0098)
#define JUMP (-0.08)

#define MAX_BLOCKNUM (200)
#define MAX_BLOCKWIDTH (10)
#define BLOCKHEIGHT (2)

//game display variables
int start_time;
int current_time = 0;
int minutes = 0;
int seconds = 0;
int lives = 10;
int score = 0;

//global variables
int screen_w;  //screen width
int screen_h;
int speed;
//sprite id
sprite_id hero;
sprite_id treasure;

//sprite image
char * hero_image =
/**/"  ()  "
/**/"--[]--"
/**/"  /\\  ";

char * hero_right=
/**/"  ()/ "
/**/" /[]  "
/**/"  /\\  ";

char * hero_left=
/**/"  \\()  "
/**/"  []\\ "
/**/"  /\\   ";

/** Treasure Sprite **/
char * treasure_1=
/**/"#$#$#"
/**/"$#$#$";

/** Define structure for blocks **/
typedef struct block_t{
    sprite_id sprite;
    int row;
    int column;
    bool safe;
} block_t;


void timer(){
    current_time = get_current_time() - start_time;
    minutes = floor(current_time/60);
    seconds = floor(current_time % 60);
    
}

void draw_display() {
  screen_w = screen_width();
  draw_line(0, 0, screen_w, 0, '~');
  draw_line(0, 4, screen_w, 4, '~');
  //display titles
  draw_formatted(3,2, "Lives: %d \t \t Time: %02d:%02d \t \t Score: %d",
lives, minutes, seconds, score);
  // draw student number
  draw_string(screen_w - 10, 2, "n9966463");

}

void setup_treasure(void){
  screen_h = screen_height();
  treasure = sprite_create(0, screen_h - 2, 5, 2, treasure_1);
  sprite_turn_to(treasure, TREASURE_SPEED, 0);
}

void setup_hero(void){
    // int start_x
    // int start_y
    hero = sprite_create(10, 10, HERO_WIDTH,HERO_HEIGHT, hero_image);
}

void draw_all(){
    clear_screen();
    sprite_draw(hero);
    sprite_draw(treasure);
    timer();
    draw_display();
    show_screen();
}

void setup( void){
    setup_hero();
    setup_treasure();
    start_time = get_current_time();
    draw_all();
}

void update_treasure(void){
  //If treasure at screen border
  
  //speed = TREASURE_SPEED;
  sprite_step(treasure);
  // int x = round(sprite_x(treasure));
  // int y = round(sprite_y(treasure));
  // if (x > 0) {
  //   speed = -speed;
  //   sprite_turn_to(treasure, speed, y);
  // }
  // else if (x < (screen_w)) {
  //   speed = -speed;
  //   sprite_turn_to(treasure, speed, y);
  // }

  
  
  
}
//gravity
void hero_gravity(void){
    // sprite_move(hero);

    //sprite location 
    // int hx = round(sprite_x(hero));
    // int hy = round(sprite_y(hero));

    double dx = sprite_dx(hero);
    double dy = sprite_dy(hero);

    dy += GRAVITY;
    sprite_turn_to(hero, dx, dy);
    sprite_step(hero);

}

//Move hero left and right
void hero_move(int key)
{
  // cab202_timer_t * idle;
  // idle create_timer( WAIT );
     //get charactor code from standard input
//   int h = screen_height();

  // screen coordinates of hero
  int x = round(sprite_x(hero));
  // int y = round(sprite_y(hero));
  //Move hero left
  if (key == 'a' && x > 0) {
    sprite_move(hero, -1, 0);
    sprite_set_image(hero, hero_left);
  }
  if (key == 'd' && x <(screen_w-6)) {
    sprite_move(hero, 1, 0);
    sprite_set_image(hero, hero_right);
  }

  if (key == 'w'){
    double dx = sprite_dx(hero);
    double dy = sprite_dy(hero);
    dy = JUMP;
    sprite_turn_to(hero, dx, dy);  
  }
}

// void treasure_move(char key)
// {
//   int x = round(sprite_x(treasure));
//   int y = round(sprite_y(treasure));



// }


int read_char()
{
    int key = paused ? wait_char() : get_char();

    if (key == 'p')
    {
        paused = !paused;
    }

    return key;
}

void process(void){

  int key = read_char();
  hero_move(key);
//  hero_gravity();
  update_treasure();
  draw_all();

}

//Clean up at end of game.
void cleanup( void ) {
	// Insert statements to release resources such as files, memory.
}

// Program entry point.
int main( void ) 
{
	setup_screen();  // Call ZDK function to prepare screen.
	setup();         // Call our setup function to initialise game state.
	show_screen();   // Display the screen at the start of play.
	// Following block will repeat until game_over becomes true.
while ( !game_over ) {
		process();

		if ( update_screen ) {
			show_screen();
		}

        timer_pause(DELAY);

    

	}
	// Game over is now true, so clean up and exit.
	cleanup();

	return 0;
}