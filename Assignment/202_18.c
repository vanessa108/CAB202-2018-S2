#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>


/** Constants **/
#define DELAY (10) //Millisecond delay between game updates
#define WAIT (1000)

#define DISPLAY_HEIGHT (5)

#define HERO_WIDTH (6)
#define HERO_HEIGHT (3)

#define TREASURE_WIDTH (5)
#define TREASURE_HEIGHT (2)
#define TREASURE_SPEED (0.8)


#define GRAVITY (0.0098)
#define JUMP_DY (-0.2)
#define JUMP_DX (0.25)

#define MAX_BLOCKNUM (200)
#define MAX_BLOCKWIDTH (10)
#define BLOCKHEIGHT (2)
#define MAX_BLOCKSPEED (0.4)
#define CHANCE_EMPTY (0.2)
#define CHANCE_FORBIDDEN (0.2)

/** game's dynamic state**/
bool game_over = false; 
bool update_screen = true;
bool toggle = true;
bool paused = false;
bool restart = false;
bool respawn = true;

/** game display variables **/
int start_time;
int current_time = 0;
int minutes = 0;
int seconds = 0;
int lives = 10;
int score = 0;

/** Block movement variables **/
double speed;
double previous_speed;
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
/**/" \\()   "
/**/" []\\  "
/**/" /\\    ";

char * hero_down=
/**/" \\()/  "
/**/" []   "
/**/" /\\   ";

char * respawn_animation[] =
{
/**/"      "
/**/"      "
/**/"  /\\  ",
/**/"      "
/**/"--[]--"
/**/"  /\\  "
};

/** Treasure Sprite **/
char * treasure_sprite[]=
{
/**/"$$$$$"
/**/"$$$$$",
/**/"#####"
/**/"#####"
};

/** Block sprite **/
char * safe_b[] = 
{
  /**/"====="
  /**/"=====",
  /**/"======"
  /**/"======",
  /**/"======="
  /**/"=======",
  /**/"========"
  /**/"========",
  /**/"========="
  /**/"=========",
  /**/"=========="
  /**/"=========="
};

char * forbidden_b[] = 
{
  /**/"XXXXX"
  /**/"XXXXX",
  /**/"XXXXXX"
  /**/"XXXXXX",
  /**/"XXXXXXX"
  /**/"XXXXXXX",
  /**/"XXXXXXXX"
  /**/"XXXXXXXX",
  /**/"XXXXXXXXX"
  /**/"XXXXXXXXX",
  /**/"XXXXXXXXXX"
  /**/"XXXXXXXXXX"
};

/** Message sprites **/
char * error_msg =
/**/	"Error: Terminal window too small.\n";

char * end_msg =
/**/	"                       GAME OVER                     "
/**/	"You are out of lives, press R to restart or Q to quit";


/** Structure for blocks **/
typedef struct block_t{
    sprite_id block_sprite;
    int block_num;
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
block_t previous_block;


/** Check the terminal window size **/
void check_screen(void) {
  //if the window dimensions are too small, display error message
  if(screen_width() <= 50 || screen_height() <= 20)
  {
    //set up message box
    int msg_height = 1;
	  int msg_width = strlen(error_msg) / msg_height;
	  sprite_id msg_box = sprite_create((screen_width() - msg_width) / 2,
      (screen_height() - msg_height) / 2, msg_width, msg_height, error_msg);
	  sprite_draw(msg_box);
	  show_screen();
    //exit game as window is too small
    game_over = true;
  }
}

/** Count time elapsed from the start of the game **/
void timer(void) {
  //find time elapsed 
  current_time = get_current_time() - start_time;
  //display elapsed time in minutes and seconds
  minutes = floor(current_time/60);
  seconds = floor(current_time % 60);
}

/** Draw display screen **/
void draw_display(void) {
  draw_line(0, 0, screen_width(), 0, '~');
  draw_line(0, 4, screen_width(), 4, '~');
  //display titles
  draw_formatted(3,2, "Lives: %d \t \t Time: %02d:%02d \t \t Score: %d",
lives, minutes, seconds, score);
  // display student number
  draw_string(screen_width() - 10, 2, "n9966463");

}

/**                       **/
/** BLOCK SETUP FUNCTIONS **/
block_parameters_t block_parameters(void);
double get_speed(int row_num, block_parameters_t b);
void gen_individual_block(individual_block_t block, bool safe, int col_num, 
int row_num, block_parameters_t b, int block_ctr);

/** Main Block Set Up Function **/
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
    //generate each individual block and assign to array 
    individual_block_t block;
    gen_individual_block(block, isSafe, col_num, row_num, b, block_ctr);
    // set block speed, direction and position
    sprite_turn_to(blocks[block_ctr].block_sprite, speed, 0);
    block_ctr++;
    }
  } break;  
  }
}

/** determine the number of rows and columns to be displayed **/
block_parameters_t block_parameters(void) {
  block_parameters_t b_param;
  b_param.row_height = HERO_HEIGHT + BLOCKHEIGHT + 3;
  b_param.num_rows = (screen_height() - DISPLAY_HEIGHT) / b_param.row_height;
  b_param.col_width = MAX_BLOCKWIDTH + 1;
  b_param.num_cols = screen_width() / b_param.col_width;
  //ensure that the bottom row of blocks does not overlap with the treasure
  if(((b_param.row_height * b_param.num_rows)+ DISPLAY_HEIGHT + HERO_HEIGHT) >= screen_height() - 1) {
    b_param.num_rows = b_param.num_rows - 1;
  }
  return b_param;
}

/** Set random speed for a row of blocks **/
double get_speed(int row_num, block_parameters_t b) {
  // generate random speed
  speed = ( (double) rand()/RAND_MAX-0.5) * 2 * MAX_BLOCKSPEED; 
  //ensure rows are moving in opposite directions
  if (previous_speed < 0) { 
    if (speed < 0) {
      speed = speed * -1;
    }
  } else if (previous_speed > 0) {
    if (speed > 0 ) {
      speed = speed * -1;
    }
  }
  previous_speed = speed;
  return speed;
}

/** Generate individual blocks **/
void gen_individual_block(individual_block_t block, bool safe, int col_num, 
int row_num, block_parameters_t b, int block_ctr) {
  int real_width[6] = {5, 6, 7, 8, 9, 10};
  // random width based on block arrays
  block.width = rand() % 5;
  block.block_image = safe? safe_b[block.width]: forbidden_b[block.width];
  //determine the x and y values based on block parameters
  block.x = col_num * b.col_width;
  block.y = (1+ row_num) * b.row_height + HERO_HEIGHT;
  //Add current block to blocks array 
  blocks[block_ctr].block_num = block_ctr;
  blocks[block_ctr].row = row_num;
  blocks[block_ctr].column = col_num;
  blocks[block_ctr].safe = safe;
  blocks[block_ctr].block_sprite = sprite_create(block.x, block.y, real_width[block.width], 
    BLOCKHEIGHT, block.block_image);
  
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
    previous_block = randblock;
    sprite_id poss = randblock.block_sprite;
    //Spawn at a random starting position
    hero = sprite_create(sprite_x(poss), sprite_y(poss) - 3, HERO_WIDTH,HERO_HEIGHT, hero_image);
    sprite_move(hero, 0, 2*JUMP_DY);
    sprite_step(hero);
}

/** Set up treasure sprite **/
void setup_treasure(void) {
  treasure = sprite_create(0, screen_height() - 2, 5, 2, treasure_sprite[0]);
  sprite_turn_to(treasure, TREASURE_SPEED, 0);
}

/**Draw all sprites and display screen **/
void draw_all(){
    clear_screen();
    sprite_draw(hero);
    sprite_draw(treasure);
    draw_blocks();
    timer();
    draw_display();
    show_screen();
}

/**Set up game components **/
void setup( void){
    setup_treasure();
    setup_blocks();
    setup_hero();
    start_time = get_current_time();
    draw_all();
}



/** Update blocks so they wrap around the screen **/
void update_blocks() {
  //loop through array of blocks
  for(size_t i = 0; i < block_ctr; i++)
  {
    sprite_id block = blocks[i].block_sprite;
    //set block in motion
    sprite_step(block);

    //get x position of block
    int x = round(sprite_x(block));
    //if block is on the edge of the screen, move to other side
    if (x < -MAX_BLOCKWIDTH) {
      sprite_move_to(block, screen_width(), block->y);
    } else if (x > screen_width()) {
      sprite_move_to(block, -MAX_BLOCKWIDTH, block->y);
    }
  }
}

/** Update treasure to move, toggle on/off **/
void update_treasure(int key) {
  //If treasure at screen border and toggle off
  speed = TREASURE_SPEED;
  if (toggle != false){
    //set treasure in motion
    sprite_step(treasure);
    int x = round(sprite_x(treasure));
    //if treasure is at the edge of the screen, reverse direction of motion
    if (x < 0) {
      speed = speed;
      sprite_turn_to(treasure, speed, 0);
    }
    else if (x > (screen_width())) {
      speed = -speed;
      sprite_turn_to(treasure, speed, 0);
    }
  } 
}

/** Sprite Position **/
sprite_pos_t sprite_pos(sprite_id sprite) {
  sprite_pos_t pos;
  pos.top = round(sprite_y(sprite));
  pos.bottom = pos.top +sprite_height(sprite);
  pos.left = round(sprite_x(sprite));
  pos.right = pos.left + sprite_width(sprite);
  return pos;
}

/** See if hero is standing on a block **/
stand_block_t hero_stand(void) {
  stand_block_t this_block;
  this_block.isStand = false;
  //loop through all blocks
  for (int i = 0; i < block_ctr; i++) {
    sprite_pos_t block_pos = sprite_pos(blocks[i].block_sprite);
    sprite_pos_t hero_pos = sprite_pos(hero);
    // if hero is standing on block, return true and block location
    if (hero_pos.bottom == block_pos.top && hero_pos.right <= block_pos.right + 3 &&  hero_pos.left >= block_pos.left - 3) {
      this_block.isStand = true;
      this_block.stand_block = blocks[i];
    } 
  } 
  return this_block;
}

/** See if hero collides with the block from the side **/
bool hero_collide(void) {
  bool isCollide = false;
  //loop through all blocks
  for (int i = 0; i < block_ctr; i++) {
    sprite_pos_t block_pos = sprite_pos(blocks[i].block_sprite);
    sprite_pos_t hero_pos = sprite_pos(hero);
    //if hero is colliding with block from the side return true
    if((hero_pos.left <= block_pos.right || hero_pos.right >= block_pos.left) && 
    ((hero_pos.top < block_pos.top && hero_pos.bottom> block_pos.top) || 
    (hero_pos.bottom > block_pos.bottom && hero_pos.top < block_pos.bottom))) {
      isCollide = true;
    }
  }
  return isCollide;
}

/** Check to see if the block the player is standing on is off the screen**/
bool block_offscreen(void) {
  block_t this_block = hero_stand().stand_block;
  sprite_pos_t block_pos = sprite_pos(this_block.block_sprite);
  //If block is offscreen, return true
    if ((block_pos.left < 0) || (block_pos.right> screen_width())) {
      return true;
    }
  return false;
}

/** Update hero's dx and dy depending on the case **/
void hero_update(void) { 
  bool isStand = hero_stand().isStand;
  block_t current_block = hero_stand().stand_block;
  //Adopt block dx if standing on block and set dy to 0
  if (isStand) {
    //sprite_move(hero, 0, JUMP_DY);
    hero->dy = 0; 
    hero->dx = current_block.block_sprite->dx;
    sprite_step(hero);
  } 
  //Player freefall when not on block
  else if (!isStand) {
    //Apply vertical acceleration due to gravity when not on block
    hero->dy += GRAVITY;
    sprite_set_image(hero, hero_down);
    sprite_step(hero);
  }
}

/** Move hero if colliding from the side **/
void hero_side(void) {
  bool isCollide = hero_collide();
  if (isCollide) {
    //move sprite in the opposite direction and set dx to 0
    sprite_move(hero, hero->dx*-1, 0);
    hero->dx = 0;
    sprite_step(hero);
  } 
}

/** Animation functions **/
int delay;
int treasure_delay;
bool image1 = true;
int respawn_delay;
int respawn_stage;

/**Change treasure_sprite each second **/
void treasure_animation(void) { 
  //count the number of static frames
  treasure_delay++;
  if (treasure_delay % 60 == 0 ) {
    //if image2 is being displayed, display image 1 and vice versa
    if (image1 == true) {
      sprite_set_image(treasure, treasure_sprite[0]);
    } else {
      sprite_set_image(treasure, treasure_sprite[1]);
    }
    image1 = !image1;
    treasure_delay = 0;
  }
}

/**Hero fades in on respawn **/
void hero_respawn_animation(void) {
  char * hero_respawn;
  // if player is respawning
  if (respawn) {
    //move through 3 stages of respawn animation
    if (respawn_delay % 30 == 0) {
      if (respawn_stage  == 0) {
        hero_respawn = respawn_animation[0];
      } else if (respawn_stage  == 1) {
        hero_respawn = respawn_animation[1];
      } else if (respawn_stage == 2){
        hero_respawn = hero_image;
        respawn = false;
      }
      sprite_set_image(hero, hero_respawn);
      //increment through respawn stages
      respawn_stage++;
    }
    //count the number of static frames
    respawn_delay++;
  }
}

/** Hero respawn after dying **/
void hero_respawn(void) { 
  //reset respawn animation varaibles
  respawn = true;
  respawn_stage = 0; 
  respawn_delay = 0;
  //destroy and recreate hero
  sprite_destroy(hero);
  setup_hero();
  
}


/**Detect if hero is colliding with treasure **/
void treasure_collide(void) {
  sprite_pos_t treasure_pos = sprite_pos(treasure);
  sprite_pos_t hero_pos = sprite_pos(hero);
  //if hero collides, treasure disappears, hero gains two lives
  if (hero_pos.bottom >= treasure_pos.top && hero_pos.left >= treasure_pos.left - 3 && hero_pos.right <= treasure_pos.right + 3) {
    lives = lives + 2;
    sprite_hide(treasure);
    hero_respawn();
  }  
}



/** Cases where player loses lives **/
void lose_lives(void) {
  bool isStand = hero_stand().isStand;
  bool isSafe = hero_stand().stand_block.safe;
  int pos_x = sprite_x(hero);
  int pos_y = sprite_y(hero);
  //if player lands on a forbidden block
  if (isStand && ! isSafe) {
    lives = lives - 1;
    hero_respawn(); 
  } 
  //if player goes off screen
  else if ((pos_x < 0 || pos_x > screen_width()) || pos_y > screen_height()) {
    lives = lives - 1;
    hero_respawn();
  } 
  //the block the player is standing on goes offscreen
  else if (isStand && block_offscreen()){
    lives = lives - 1;
    hero_respawn();
  }
}

/**Cases for scoring points **/
void score_block(void) {
  bool isStand = hero_stand().isStand;
  bool isSafe = hero_stand().stand_block.safe;
  block_t  current_block = hero_stand().stand_block;
  //if a player lands on a block that is not the previous block, gain point
  //i.e jumps on and down on the same block
  if (isSafe && isStand && previous_block.block_num != current_block.block_num){
    previous_block = current_block;
    score++;
  }
}

/** Hero movement from keyboard input**/
void hero_move(int key) {
  bool isStand = hero_stand().isStand;
  int x = round(sprite_x(hero));
  //hero jumps when on a block
  if (key == 'w' && isStand){
    hero->dy = JUMP_DY;
    sprite_step(hero);
  } 
  //hero moves left when a is pressed and on a block
  else if (key == 'a' && x > 0 && isStand) {
    sprite_move(hero, -1, 0);
    sprite_set_image(hero, hero_left);
  }
  //hero moves right when a is pressed and on a block
  else if (key == 'd' && x <(screen_width()-6) && isStand) {
    sprite_move(hero, 1, 0);
    sprite_set_image(hero, hero_right);
  }
  //player has constant horziontal velocity when jumping 
  else if (key == 'a' && x > 0 && !isStand){
    hero->dx += -JUMP_DX;
    sprite_step(hero);
  }
  else if (key == 'd' && x <(screen_width()-6) && !isStand) {
    hero->dx += JUMP_DX;
    sprite_step(hero);
  } 
  //reset player sprite image to default when player is not moving
  else {
    //count number of static frames
    delay++;
    if (delay % 50 == 0 && !respawn) {
      sprite_set_image(hero, hero_image);
      delay = 0;
    }
  }
}

/** get input from the keyboard (from ZDK example) **/
int read_char(void) {   
  //either wait for input or read input
  int key = paused ? wait_char() : get_char();
  //toggle for treasure
  if (key == 't') {
    toggle = !toggle;
  }
  return key;
}

int main(void);

/**clear screen and variables when game resets **/
void reset_game(void) {
  clear_screen();
  respawn = true;
  respawn_stage = 0; 
  respawn_delay = 0;
  paused = false;
  toggle = true;
  current_time = 0;
  minutes = 0;
  seconds = 0;
  lives = 10;
  score = 0;
  //run main to start game again
  main();
}


/**display game over screen when lives = 0 **/
void gameover_screen(void) {
  //pause the game functions
  paused = true; 
  clear_screen();
  // Get dimensions and coordinates of message text.
  int msg_height = 2;
  int msg_width = strlen(end_msg) / msg_height;
  int msg_left = (screen_width() - msg_width) / 2;
  int msg_top = (screen_height() - msg_height) / 2;
  //create message box
  sprite_id msg = sprite_create( msg_left, msg_top, msg_width, msg_height, end_msg);
  // draw message box, including score and time elapsed
  draw_formatted(msg_left+ 20, msg_top + 3, "Score:         %d", score);
  draw_formatted(msg_left+ 20, msg_top + 4, "Elapsed Time:  %02d:%02d", minutes, seconds);
  sprite_draw(msg);
  show_screen();
  //wait for key input
  int key = read_char();
  //if q, quit game
  if (key == 'q' || key == 'Q') {
    game_over = true;
  }
  // if r, restart game
  if (key == 'r' || key == 'R'){
    reset_game();
  }
}

//Processes required to run the game
void process(void) {
  int key = read_char();
  hero_move(key);
  hero_update();
  update_treasure(key);
  treasure_animation();
  hero_side();
  update_blocks();
  draw_all();
  score_block();
  lose_lives();
  treasure_collide();
  hero_respawn_animation();
  if (lives <=0) {
    gameover_screen();
    return;
  }
}

// Program entry point.
int main( void ) 
{ 
	setup_screen();  
  check_screen();
	setup();         // Call our setup function to initialise game state.
	show_screen();   // Display the screen at the start of play.
	//Repeat process until game over 
  while ( !game_over) {
		process();

		if ( update_screen ) {
			show_screen();
		}
      timer_pause(DELAY);

	}
  //exit game when game over is true
	return 0;

}
