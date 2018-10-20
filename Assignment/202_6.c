#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

//gcc 202_6.c -std=gnu99 -Wall -Werror -Ic:/cab202/ZDK -Lc:/cab202/ZDK -lZDK -lncurses -o 202
//game's dynamic state
bool game_over = false; 
bool update_screen = true;
bool toggle = true;
bool paused = false;

/** Constants **/
#define DELAY (10) //Millisecond delay between game updates
#define WAIT (1000)

#define DISPLAY_HEIGHT (5)

#define HERO_WIDTH (6)
#define HERO_HEIGHT (3)

#define TREASURE_WIDTH (5)
#define TREASURE_HEIGHT (2)
#define TREASURE_SPEED (1)

#define GRAVITY (0.0098)
#define JUMP (-0.4)

#define MAX_BLOCKNUM (200)
#define MAX_BLOCKWIDTH (10)
#define BLOCKHEIGHT (2)
#define MAX_BLOCKSPEED (0.5)
#define CHANCE_EMPTY (0.2)
#define CHANCE_FORBIDDEN (0.2)
//Block variables

//game display variables
int start_time;
int current_time = 0;
int minutes = 0;
int seconds = 0;
int lives = 10;
int score = 0;

int block_ctr;
//global variables
int screen_w;  //screen width
int screen_h;
double speed;
//sprite id
sprite_id hero;
sprite_id treasure;


// timer_id idle;
/** Global variables for blocks (MAKE STRUCT) **/

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

// char * hero_up=
// /**/"  \\()/  "
// /**/"  []\ "
// /**/"  /\\  ";

/** Treasure Sprite **/
char * treasure_1=
/**/"#$#$#"
/**/"$#$#$";

/** Block sprite **/
char * safe_b[] = 
{
  /**/"   =====  "
  /**/"   =====  ",
  /**/"  ======  "
  /**/"  ======  ",
  /**/" =======  "
  /**/" =======  ",
  /**/" ======== "
  /**/" ======== ",
  /**/"========= "
  /**/"========= ",
  /**/"=========="
  /**/"=========="
};

char * forbidden_b[] = 
{
  /**/"   XXXXX  "
  /**/"   XXXXX  ",
  /**/"  XXXXXX  "
  /**/"  XXXXXX  ",
  /**/" XXXXXXX  "
  /**/" XXXXXXX  ",
  /**/" XXXXXXXX "
  /**/" XXXXXXXX ",
  /**/"XXXXXXXXX "
  /**/"XXXXXXXXX ",
  /**/"XXXXXXXXXX"
  /**/"XXXXXXXXXX"
};


/** Define structure for blocks **/
typedef struct block_t{
    sprite_id block_sprite;
    int row;
    int column;
    //bool safe;
} block_t;

typedef struct block_parameters_t{
  int row_height;
  int num_rows;
  int col_width;
  int num_cols;
} block_parameters_t;

typedef struct individual_block_t {
  int x;
  int y;
  int width;
  char * block_image;
} individual_block_t;

block_t blocks[MAX_BLOCKNUM];
//block_parameters_t b;

void timer(){
  current_time = get_current_time() - start_time;
  minutes = floor(current_time/60);
  seconds = floor(current_time % 60);
}

void screen_parameters(void){
  screen_w = screen_width();
  screen_h = screen_height();
}

void draw_display() {
  draw_line(0, 0, screen_w, 0, '~');
  draw_line(0, 4, screen_w, 4, '~');
  //display titles
  draw_formatted(3,2, "Lives: %d \t \t Time: %02d:%02d \t \t Score: %d",
lives, minutes, seconds, score);
  // draw student number
  draw_string(screen_w - 10, 2, "n9966463");

}

void setup_treasure(void){
  treasure = sprite_create(0, screen_h - 2, 5, 2, treasure_1);
  sprite_turn_to(treasure, TREASURE_SPEED, 0);
}

void setup_hero(void){
    // int start_x
    // int start_y
    hero = sprite_create(10, 6, HERO_WIDTH,HERO_HEIGHT, hero_image);
}
/** BLOCK SETUP FUNCTIONS **/
block_parameters_t block_parameters(void){
  block_parameters_t b_param;
  b_param.row_height = HERO_HEIGHT + BLOCKHEIGHT + 2;
  b_param.num_rows = (screen_h - DISPLAY_HEIGHT) / b_param.row_height;
  b_param.col_width = MAX_BLOCKWIDTH + 1;
  b_param.num_cols = screen_w / b_param.col_width;

  return b_param;
}


double get_speed(int row_num, block_parameters_t b){
  if(row_num > 0 && row_num < (b.num_rows -1)){
    speed = ( (double) rand()/RAND_MAX-0.5) * 2 * MAX_BLOCKSPEED;
  } 
  return speed;
}

void gen_individual_block(individual_block_t block, bool safe, int col_num, 
int row_num, block_parameters_t b, int block_ctr) {
  block.width = rand() % 5;
  block.block_image = safe? safe_b[block.width]: forbidden_b[block.width];
  block.x = col_num * b.col_width;
  block.y = (1+ row_num) * b.row_height + HERO_HEIGHT - 1;
  
  blocks[block_ctr].row = row_num;
  blocks[block_ctr].column = col_num;
  blocks[block_ctr].block_sprite = sprite_create(block.x, block.y, MAX_BLOCKWIDTH, BLOCKHEIGHT, block.block_image);
  
}




void setup_blocks(void){
  block_parameters_t b;
  b = block_parameters();
  

  block_ctr = 0;
  
  while(block_ctr < 200){
      for (int row_num = 0; row_num < b.num_rows; row_num++)
  {
    int safe_ctr = 0;

    speed = 0;
    if (row_num > 0 && row_num < (b.num_rows-1)) {
      speed = get_speed(row_num, b);
    }
    
    for (int col_num = 0; col_num < b.num_cols; col_num++)
    {
      bool isSafe = true;

      if (col_num == (b.num_cols -1) && safe_ctr == 0){
        isSafe = true;
      } else {
        if ( (double) rand() / RAND_MAX < CHANCE_EMPTY) {
          continue;
        }
      isSafe = ( (double) rand()/ RAND_MAX > CHANCE_FORBIDDEN);
      safe_ctr++;
    }

    individual_block_t block;
    gen_individual_block(block, isSafe, col_num, row_num, b, block_ctr);
  
    sprite_turn_to(blocks[block_ctr].block_sprite, speed, 0);
    block_ctr++;
    }
  } break;
    
  }
}
  

void draw_blocks() {
  
  for(size_t i = 0; i < block_ctr; i++)
  {
    sprite_id block = blocks[i].block_sprite;
    sprite_draw(block);
  }
  
}

void update_blocks() {
  for(size_t i = 0; i < block_ctr; i++)
  {
    sprite_id block = blocks[i].block_sprite;
    sprite_step(block);

    int x = round(sprite_x(block));
    int y = round(sprite_y(block));

    if (x < -MAX_BLOCKWIDTH) {
      sprite_move_to(block, screen_w, y);
    } else if (x > screen_w) {
      sprite_move_to(block, -MAX_BLOCKWIDTH, y);
    }


  }
  
}


void draw_all(){
    clear_screen();
    sprite_draw(hero);
    sprite_draw(treasure);
    draw_blocks();
    timer();
    draw_display();
    show_screen();
}

void setup( void){
  screen_parameters();
    setup_hero();
    setup_treasure();
    setup_blocks();
    start_time = get_current_time();
    draw_all();
}

void update_treasure(int key){
  //If treasure at screen border
  speed = TREASURE_SPEED;
  if (toggle != false){
    sprite_step(treasure);
    int x = round(sprite_x(treasure));
  // int y = round(sprite_y(treasure));
    if (x < 0) {
      speed = speed;
      sprite_turn_to(treasure, speed, 0);
    }
    else if (x > (screen_w)) {
      speed = -speed;
      sprite_turn_to(treasure, speed, 0);
    }

  }
  
 
  
}
// gravity
// void hero_gravity(void){
//     // sprite_move(hero);

//     //sprite location 
//     // int hx = round(sprite_x(hero));
//     // int hy = round(sprite_y(hero));

//     double dx = sprite_dx(hero);
//     double dy = sprite_dy(hero);

//     dy += GRAVITY;
//     sprite_turn_to(hero, dx, dy);
//     sprite_step(hero);

// }

//Move hero left and right
void hero_move(int key)
{
 
  // screen coordinates of hero
  int x = round(sprite_x(hero));
  //int y = round(sprite_y(hero));
  // int y = round(sprite_y(hero));
  double dx = sprite_dx(hero);
  double dy = sprite_dy(hero);
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
    dy = JUMP;
    //sprite_set_image(hero, hero_up);
    //sprite_turn_to(hero, dx, dy);
  } else {
    dy += GRAVITY;
    }

  // if (key == 'w' && key == 'a'){
  //   dy = JUMP;
  //   dx = 1;
  //   //sprite_set_image(hero, hero_up);
  //   //sprite_turn_to(hero, dx, dy);
  // } else {
  //   dy += GRAVITY;
  //   }


  sprite_turn_to(hero, dx, dy);
  sprite_step(hero);
}



int read_char()
{
    int key = paused ? wait_char() : get_char();
    if (key == 't')
    {
      toggle = !toggle;
    }

    return key;
}

void process(void){

  int key = read_char();
  hero_move(key);
//  hero_gravity();
  update_treasure(key);
  update_blocks();
  draw_all();

  //printf("%d", block_ctr);

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