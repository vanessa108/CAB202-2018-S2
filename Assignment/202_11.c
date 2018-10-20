#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

//gcc 202_10.c -std=gnu99 -Wall -Werror -Ic:/cab202/ZDK -Lc:/cab202/ZDK -lZDK -lncurses -o 202

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
#define JUMP (-0.3)

#define MAX_BLOCKNUM (200)
#define MAX_BLOCKWIDTH (10)
#define BLOCKHEIGHT (2)
#define MAX_BLOCKSPEED (0.5)
#define CHANCE_EMPTY (0.2)
#define CHANCE_FORBIDDEN (0.2)

/** game's dynamic state**/
bool game_over = false; 
bool update_screen = true;
bool toggle = true;
bool paused = false;

/** game display variables **/
int start_time;
int current_time = 0;
int minutes = 0;
int seconds = 0;
int lives = 10;
int score = 0;


/** misc global variables **/
int screen_w;  //screen width
int screen_h;
double speed;
int block_ctr;

/** sprite id **/
sprite_id hero;
sprite_id treasure;

/** Sprite Images **/
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


/** Structure for blocks **/
typedef struct block_t{
    sprite_id block_sprite;
    int row;
    int column;
    bool safe;
} block_t;

/** Structure for block parameters **/
typedef struct block_parameters_t{
  int row_height;
  int num_rows;
  int col_width;
  int num_cols;
} block_parameters_t;

/** Structure for generating individual blocks **/
typedef struct individual_block_t {
  int x;
  int y;
  int width;
  char * block_image;
} individual_block_t;

/** Structure for a sprite's position **/
typedef struct sprite_pos_t {
  int top;
  int bottom;
  int left;
  int right;
} sprite_pos_t;

/** Structure for determining if a hero is standing on a block **/
typedef struct stand_block_t {
  block_t stand_block;
  bool isStand;
} stand_block_t;

/** Array of blocks **/
block_t blocks[MAX_BLOCKNUM];



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




/** BLOCK SETUP FUNCTIONS **/
block_parameters_t block_parameters(void){
  block_parameters_t b_param;
  b_param.row_height = HERO_HEIGHT + BLOCKHEIGHT + 5;
  b_param.num_rows = (screen_h - DISPLAY_HEIGHT) / b_param.row_height;
  b_param.col_width = MAX_BLOCKWIDTH + 1;
  b_param.num_cols = screen_w / b_param.col_width;

  return b_param;
}

/** Set random speed for a row of blocks **/
double get_speed(int row_num, block_parameters_t b){
  speed = ( (double) rand()/RAND_MAX-0.5) * 2 * MAX_BLOCKSPEED; 
  return speed;
}

/** Generate individual blocks **/
void gen_individual_block(individual_block_t block, bool safe, int col_num, 
int row_num, block_parameters_t b, int block_ctr) {
  block.width = rand() % 5;
  block.block_image = safe? safe_b[block.width]: forbidden_b[block.width];
  block.x = col_num * b.col_width;
  block.y = (1+ row_num) * b.row_height + HERO_HEIGHT - 1;
  //Add current block to blocks array
  blocks[block_ctr].row = row_num;
  blocks[block_ctr].column = col_num;
  blocks[block_ctr].safe = safe;
  blocks[block_ctr].block_sprite = sprite_create(block.x, block.y, MAX_BLOCKWIDTH, BLOCKHEIGHT, block.block_image);
  
}

/** Setup blocks **/
void setup_blocks(void){
  block_parameters_t b;
  b = block_parameters();
  //random seed
  int now = get_current_time();
  srand( now );
  block_ctr = 0;
  
  //loop through rows
  while(block_ctr < MAX_BLOCKNUM){
      for (int row_num = 0; row_num < b.num_rows; row_num++)
  {
    int safe_ctr = 0;
    speed = 0;
    //set random speed for middle rows
    if (row_num > 0 && row_num < (b.num_rows-1)) {
      speed = get_speed(row_num, b);
    }
    //loop through columns
    for (int col_num = 0; col_num < b.num_cols; col_num++)
    {
      bool isSafe = true;
      // force one block in the column to be safe
      if (col_num == (b.num_cols -1) && safe_ctr == 0){
        isSafe = true;
      } else {
        //randomise blocks: empty, safe, forbidden
        if ( (double) rand() / RAND_MAX < CHANCE_EMPTY) {
          continue;
        }
      isSafe = ( (double) rand()/ RAND_MAX > CHANCE_FORBIDDEN);
      safe_ctr++;
    }

    individual_block_t block;
    gen_individual_block(block, isSafe, col_num, row_num, b, block_ctr);
    // set block speed, direction and position
    sprite_turn_to(blocks[block_ctr].block_sprite, speed, 0);
    block_ctr++;
    }
  } break;
    
  }
}


  
/** Draw  blocks **/
void draw_blocks() {
  //loop through block array
  for(size_t i = 0; i < block_ctr; i++)
  {
    sprite_id block = blocks[i].block_sprite;
    sprite_draw(block);
  }
  
}

/**Set up hero to randomly spawn on safe block **/
void setup_hero(void) {
    // store an array of possible starting blocks
    block_t possible[block_ctr];
    int possible_ctr = 0;
    for (int i = 0; i < block_ctr; i++){
      if (blocks[i].row == 0 && blocks[i].safe == true){
        possible[possible_ctr] = blocks[i];
        possible_ctr++;
      }
    }
    // Choose a random starting block
    int now = get_current_time();
    srand(now);
    int x = (rand() % possible_ctr);
    block_t randblock = possible[x];
    sprite_id poss = randblock.block_sprite;
    //Spawn at random starting position
    hero = sprite_create(sprite_x(poss), sprite_y(poss) - 3, HERO_WIDTH,HERO_HEIGHT, hero_image);
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
    setup_treasure();
    setup_blocks();
    setup_hero();
    start_time = get_current_time();
    draw_all();
}



/** Update blocks so they wrap around the screen **/
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

/** Update treasure to move, toggle on/off **/
void update_treasure(int key){
  //If treasure at screen border and toggle off
  speed = TREASURE_SPEED;
  if (toggle != false){
    sprite_step(treasure);
    int x = round(sprite_x(treasure));
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

/** Sprite Position **/
sprite_pos_t sprite_pos(sprite_id sprite){
  sprite_pos_t pos;
  pos.top = round(sprite_y(sprite));
  pos.bottom = pos.top +sprite_height(sprite);
  pos.left = round(sprite_x(sprite));
  pos.right = pos.left + sprite_width(sprite);
  return pos;
}

/** See if hero is standing on a block **/
stand_block_t hero_stand(){

  stand_block_t this_block;
  this_block.isStand = false;
  for (int i = 0; i < block_ctr; i++) {
    block_t block = blocks[i];
    sprite_pos_t block_pos = sprite_pos(block.block_sprite);
    sprite_pos_t hero_pos = sprite_pos(hero);

    // if hero is standing on block, return true and block location
    if (hero_pos.bottom == block_pos.top && hero_pos.right <= block_pos.right + 1 &&  hero_pos.left >= block_pos.left - 2) {
      this_block.isStand = true;
      this_block.stand_block = blocks[i];
    } 
  } 
  return this_block;
}

/** Update hero's dx and dy depending on block **/
void hero_update(){ 
  bool isStand = hero_stand().isStand;
  block_t current_block = hero_stand().stand_block;

  //Adopt block dx if standing on block
  if (isStand) {
    hero->dy = 0; 
    hero->dx = current_block.block_sprite->dx;
  } else {
    //Apply vertical acceleration due to gravity when not on block
    hero->dy += GRAVITY;
  }
  sprite_step(hero);
}

/** Hero respawn after dying **/
void hero_respawn(){ 
  sprite_destroy(hero);
  setup_hero();
}

/**Detect if hero is colliding with treasure **/
void treasure_collide() {
  sprite_pos_t treasure_pos = sprite_pos(treasure);
  sprite_pos_t hero_pos = sprite_pos(hero);
  //if hero collides, treasure disappears, hero gains two lives
  if (hero_pos.bottom >= treasure_pos.top && hero_pos.left >= treasure_pos.left - 3 && hero_pos.right <= treasure_pos.right + 3) {
    lives = lives + 2;
    sprite_hide(treasure);
    hero_respawn();
  }  
}


void lose_lives() {
  bool isStand = hero_stand().isStand;
  bool isSafe = hero_stand().stand_block.safe;
  int pos_x = sprite_x(hero);
  int pos_y = sprite_y(hero);

  if (isStand && ! isSafe) {
    lives = lives - 1;
    sprite_destroy(hero);
    setup_hero();
  } else if ((pos_x < 0 || pos_x > screen_w) || pos_y > screen_h) {
    lives = lives - 1;
    sprite_destroy(hero);
    setup_hero();
  } 
}
  
// }

// void score_block(){
//   bool isStand = hero_stand().isStand;
//   bool isSafe = hero_stand().stand_block.safe;
//   block_t previous_block = hero_stand().stand_block;
//   if (isStand && isSafe){
//     block_t current_block = hero_stand().stand_block;
//     if ((current_block.row != previous_block.row) && (current_block.column != previous_block.column)){
//       score++;
//       previous_block = current_block;
//     }
//   }
// }
//Move hero left and right
void hero_move(int key)
{
  //bool isStand = hero_stand().isStand;
  // screen coordinates of hero
  int x = round(sprite_x(hero));

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
    hero->dy = JUMP;
    sprite_step(hero);
    //sprite_set_image(hero, hero_up);
  } 
  
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
  hero_update();
  update_treasure(key);
  update_blocks();
  draw_all();
  //hero_score();
  lose_lives();
  treasure_collide();

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