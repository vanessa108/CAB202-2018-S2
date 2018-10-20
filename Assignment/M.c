#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>


/////////////////////////////////////// Constants ///////////////////////////////////////
#define DELAY (10) /* Millisecond delay between game updates */

#define PLAYER_WIDTH (6)
#define PLAYER_HEIGHT (3)
#define TREASURE_WIDTH (3)
#define TREASURE_HEIGHT (1)
#define GAMEOVERMESSAGE_WIDTH (40)
#define GAMEOVERMESSAGE_HEIGHT (4)
#define DISPLAY_MARGIN (3)
#define DISPLAY_HEIGHT (4)
#define COLUMN_WIDTH (12)
#define GRAVITY (0.01)
#define JUMP (0.4)


// Block struct 
typedef struct Block {
        bool safe;
		int row;
		int width;
		int x, y;
		double dx;
		char * bitmap;
} Block;

/////////////////////////////////////// Game state ///////////////////////////////////////
bool game_over = false; /* Set this to true when game is over */
bool update_screen = true; /* Set to false to prevent screen update. */

///////////////////////////////////////   Images  ///////////////////////////////////////
char * player_stable =
/**/	" {o,o}"
/**/	"  (_) "
/**/	"  \" \" ";

char * jump_right =
/**/	" {^,^}"
/**/	"./)_) "
/**/	"  \" \" ";

char * jump_left =
/**/	"{^,^} "
/**/	" (_(\\."
/**/	"  \" \" ";

char * player_dead =
/**/	" {x,x}"
/**/	"  (_) "
/**/	"  \" \" ";

char * player_right =
/**/	" {o,o}"
/**/	"./)_) "
/**/	"  \" \" ";

char * player_left =
/**/	"{o,o} "
/**/	" (_(\\."
/**/	"  \" \" ";

char * treasure_image_front =
/**/	"[$]";

char * treasure_image_back =
/**/	"[@]";

char * block_images[12] = {
/**/	"====="
/**/	"=====",
/**/	"======"
/**/	"======",
/**/	"======="
/**/	"=======",
/**/	"========"
/**/	"========",
/**/	"========="
/**/	"=========",
/**/	"=========="
/**/	"==========",
/**/	"xxxxx"
/**/	"xxxxx",
/**/	"xxxxxx"
/**/	"xxxxxx",
/**/	"xxxxxxx"
/**/	"xxxxxxx",
/**/	"xxxxxxxx"
/**/	"xxxxxxxx",
/**/	"xxxxxxxxx"
/**/	"xxxxxxxxx",
/**/	"xxxxxxxxxx"
/**/	"xxxxxxxxxx"};

char * game_over_message =
/**/	"         You are out of lives!!         "
/**/	"         Your total score was %3d       "
/**/	" You played for a total time of %2d:%2d.  " 
/**/	" Press 'r' to restart or 'q' to quit :) ";

char formatted_game_over_screen[4];
// Declare a sprite_id called player.
sprite_id player;

// Declare a sprite_id called treasure.
sprite_id treasure;

// Declare a sprite_id called message.
sprite_id message;

/////////////////////////////////////// Global variables and switches ///////////////////////////////////////
int lives = 10;
bool dead = false;

int score = 0;
bool scored = false;

int mins = 0;
int secs = 0;
double start_time = 0;

int max_rows;
int num_blocks = 0;
Block blocks[200];
sprite_id block_sprites[200];
int starting_block[20];
int first_row;

int prev_block = 0;
int act_block;

bool paused = false;
bool direc = false;

bool stop_game = false;
int keyCode; 

timer_id stable; 

/////////////////////////////////////// Set- up functions  ///////////////////////////////////////

void draw_display(double Start_Time){				// Draws display screen 
	draw_line(0, 0, screen_width()-1, 0, '*');		// Draw border
    draw_line(0, DISPLAY_HEIGHT, screen_width()-1, DISPLAY_HEIGHT, '*');
    draw_line(0, 0, 0,DISPLAY_HEIGHT, '*');
    draw_line(screen_width()-1, 0, screen_width()-1, DISPLAY_HEIGHT, '*');

	draw_string(screen_width()-25-DISPLAY_MARGIN, DISPLAY_HEIGHT/2, "Student Number: n9949780");		// Student number 
	
	int space_between_displays = (screen_width() - 56 - (2*DISPLAY_MARGIN))/5; 							// Lives 
	draw_formatted(DISPLAY_MARGIN, 5/2,"Lives: %d ", lives);
	
	int time = get_current_time() - Start_Time;
	mins = round(time / 60);
	secs = (time) % 60;
	draw_formatted(DISPLAY_MARGIN+space_between_displays+9, DISPLAY_HEIGHT/2,"Time: %02d:%02d", mins, secs);	// Time 
	
	draw_formatted(DISPLAY_MARGIN+(2*space_between_displays)+20, DISPLAY_HEIGHT/2,"Score: %d", score);			// Score 
}

bool can_draw(int Col_Num, int Row_Num) {			// Check if no block exists at location Col_Num, Row_Num
	for (int ctr = 0; ctr < num_blocks; ctr++)
	{
		if (blocks[ctr].x == Col_Num && 
			blocks[ctr].y == Row_Num ){
				return false;
			}
	}	
	return true;
}

void create_block(int Col_Num){						// Creates a block at the given Col_Num
	blocks[num_blocks].width = (rand() % (6));		// Random width between of the 5 options (5 to 10)
	blocks[num_blocks].row = (rand() % (max_rows+1))+1;
	int row_num = ( blocks[num_blocks].row)*(PLAYER_HEIGHT + 4) + DISPLAY_HEIGHT ; 		// Random row 
	if (can_draw(Col_Num, row_num)){		// Checks if position is available 
			blocks[num_blocks].x = Col_Num;
			blocks[num_blocks].y = row_num;
			num_blocks++;
	}
}

void display_blocks(void){							// Place blocks on the screen 
	int columns = floor(screen_width()/COLUMN_WIDTH); //SCREEN WIDTH - max block width and space between blocks
	max_rows = ((screen_height() - DISPLAY_HEIGHT - (TREASURE_HEIGHT)-2)/(PLAYER_HEIGHT + 6));
	srand(start_time);
	for (int i = 0; i <= (2 * columns); i++){		// Draw one to two safe blocks per column 
		blocks[num_blocks].safe = true;
		int col_num = ((i%columns)*(COLUMN_WIDTH) + 1);
        create_block(col_num);		}
	for (int i = 0; i < 3; i++)	{ 					// Draw at least two forbidden blocks
		blocks[num_blocks].safe = false;
		int col_num = ((rand() % (columns))*(COLUMN_WIDTH)+1);
		create_block(col_num);		}
	int b = (rand() % (columns +1)); 				// Random number of blocks to draw
	for (int i = 0; i <= b+4; i++){
		if (rand() % (2) == 0){						// Randomise if its safe or forbidden 
            blocks[num_blocks].safe = false ;		}
		else {
			blocks[num_blocks].safe = true;			}
		int col_num = ((rand() % (columns))*(COLUMN_WIDTH)+1);		 // Randomise column position
		create_block(col_num);		}
}

void draw_blocks (int num_blocks){					// Create block spites and draw on screen
	for (int a = 0; a < num_blocks ; a++){			
        int i;
        if (blocks[a].safe == false)				// Checks which image it must use depending on width and if safe
        {
            i = blocks[a].width + 6;
        }
        else {
            i = blocks[a].width; 
        }
		block_sprites[a] = sprite_create (blocks[a].x, blocks[a].y ,blocks[a].width +5, 
        2, block_images[i]);						// Create sprite 
		sprite_draw(block_sprites[a]);				// Place on screen 
	}
}

void block_speed(void){								// Assigns each block a speed depending on its row
	double speed;
	for (int ctr = 0; ctr < num_blocks; ctr++){
		if ((blocks[ctr].row == 1) || (blocks[ctr].row == max_rows+1)){
			blocks[ctr].dx = 0.0;
		}
		else{
			speed = blocks[ctr].row * 0.05;
			if (blocks[ctr].row%2 == 0){			// inverses speed when its an even row 
				speed  = -speed;
			}
			blocks[ctr].dx = speed;
		}
	}
}

void move_blocks (void){							// Update position of blocks according to assigned speed 
	for(int ctr = 0; ctr < num_blocks; ctr++){
		if ((blocks[ctr].row != 1) || (blocks[ctr].row != max_rows+1)){
			sprite_turn_to(block_sprites[ctr], blocks[ctr].dx, 0.0);
		}
		if (block_sprites[ctr]->x > screen_width()){		// Keeps blocks rotating around screen if they reach the end 
			block_sprites[ctr]->x = 0;
		}
		if (block_sprites[ctr]->x < 0 ){
			block_sprites[ctr]->x = screen_width();
		}
    	sprite_step(block_sprites[ctr]);
		sprite_draw(block_sprites[ctr]);
	}
}

int first_row_blocks(void){	
	int i = 0;										// Finds all safe blocks in the first row
	for (int ctr = 0; ctr < num_blocks;ctr++){		// Loops through blocks to find all safe blocks in the first row
		if (blocks[ctr].row == 1 && blocks[ctr].safe == true ){
			starting_block[i] = ctr;				// Places number of block which is safe in an array
			i++;
		}
	}
	return i;										// Returns the max number of safe blocks in the top row
}

void place_player(int first_row ){  				// Create player sprite starting at random block on top row 
	int x, y;
	int first_row_ctr = rand() % first_row;
	y = blocks[starting_block[first_row_ctr]].y - PLAYER_HEIGHT -1 ;
	x = blocks[starting_block[first_row_ctr]].x + 1; 
	if (player == NULL){
		player = sprite_create(x, y, PLAYER_WIDTH, PLAYER_HEIGHT, player_stable);
	} else{
		sprite_set_image(player, player_stable);
		sprite_move_to(player, x, y);
	}
}

void setup_treasure(void){							// Set up the treasure 
	treasure = sprite_create( 0 , screen_height() - TREASURE_HEIGHT, TREASURE_WIDTH, TREASURE_HEIGHT, treasure_image_front);
	sprite_draw(treasure);
    sprite_turn_to(treasure, 0.01 , 0.0);			// Initialise treasure movement 
    sprite_step(treasure);
}

/////////////////////////////////////// End of Game Functions ///////////////////////////////////////

void reset(void){									// Reset the game
	clear_screen();
	start_time = get_current_time();				// Reset all globals and arrays
	memset(block_sprites,' ',200);
	memset(blocks, ' ', 200);
	num_blocks = 0;
	dead = false;
	lives = 10;
	score = 0;
	draw_display(start_time);						// Reset display screen
	display_blocks();								// Set up blocks 
	block_speed();
	first_row = first_row_blocks();
	place_player( first_row);				// Set up the player at random block on top row.
	sprite_draw(player);							// Draw the player and blocks
	draw_blocks(num_blocks);				
	show_screen();
}

void game_over_screen (void){						// Display Game Over message
	int x = (screen_width()-GAMEOVERMESSAGE_WIDTH)/2;							// Locate message at centre of screen
	int y = (screen_height()-GAMEOVERMESSAGE_HEIGHT)/2;
	sprintf(formatted_game_over_screen,game_over_message,score,mins,secs);		// Format message with score and time 
	message  = sprite_create(x, y, 40, 4, formatted_game_over_screen);			
	sprite_draw(message);														// Place message on screen 
	show_screen();
	while ( stop_game )	{														// Keyborad input options 
		keyCode = get_char(); 
		if (keyCode == 'q' ){
			stop_game = false;													// Ends game and quits gracefully
			game_over = true;
			return;
		}
		if (keyCode == 'r' ){
			stop_game = false;													// Completely restarts the game 
			reset();
		}clear_screen();
	}
}

void player_die (void){								// 	Takes away a life everytime player dies
	if (lives == 1){
		clear_screen();
		stop_game = true;
		game_over_screen();							// If no more lifes left, call game over screen 
	}
	else{
		lives -= 1;
		sprite_set_image(player, player_dead);
	}
}

/////////////////////////////////////// Process Functions ///////////////////////////////////////

bool on_block_checker (int a){						// Check if player is on a block and returns bool 
	for (int ctr = 0; ctr < a; ctr++){				// Loops through every block 
		if (round(sprite_y(player)) == block_sprites[ctr]->y - sprite_height(player)){	// Checks if location of player
			if (((round(player->x) >= block_sprites[ctr]->x +1) || 						// is right above a block
			((round(player->x) + PLAYER_WIDTH ) > block_sprites[ctr]->x + 1)) 			// and within the x direction
			&& round(player->x) < (block_sprites[ctr]->x + block_sprites[ctr]->width)){ // limits of the block.
				if (dead){							
					return false;				// Ignores block if player already dead.
				}	else{							
					act_block = ctr;			// Stores the block is it on to respawn at different block next time.
					if (act_block != prev_block && !scored){
						if (blocks[act_block].safe == false){	// If block is forbidden, declare player dead.
							dead = true;		
						}	else {							// Give player a point if point hasn't already been given.
							score++;	}
						prev_block = act_block;
						scored = true;
					}	else{
						scored = false;		}
				return true;								
				}	}	}	}	return false;
}

void catch_treasure(void){							// Checks for pixel level collision between player and treasure 
	for (int x1 = 0; x1 < player->width; x1++){		// Check through location of each pixel of both tresure and player to check for collision 
	for ( int y1 = 0; y1 < player->height; y1++){
	for (int x2 = 0; x2 < treasure->width; x2++){
	for (int y2 = 0; y2 < treasure->height; y2++){
			int offset1 = y1 * (player->width) + x1;	
			int offset2 = y2 * (treasure->width) + x2;
			if (player->bitmap[offset1] != ' ' && 		// Check colliding pixels contain a character and aren't empty 
				treasure->bitmap[offset2] != ' ' &&
				round(x2+treasure->x )== round(x1+player->x) && 
				round(y2+treasure->y) == round(y1+player->y)){	
					if (!dead){							// Only works when player isn't dead
							player->dy = 0.0;
							lives = lives + 2;			// Assigns 2 more lives 
							treasure->is_visible = false;
							place_player(first_row);	// Player respawns 
						}
			}
	}	}	}	}
}

void treasure_movement(void){						// Moves the treasure across the screen 	
	if (paused){
        treasure->dx = 0.0;
		if (treasure->bitmap == treasure_image_front){			// Treasure animation; changes image when paused 
			treasure->bitmap = treasure_image_back;}
	}	else {
			if (treasure->bitmap != treasure_image_front){
				treasure->bitmap = treasure_image_front ;}
			if(!direc){								// Assigns horizontal speed depending on direction 
				treasure->dx = 0.2;
			}	else {
				treasure->dx = -0.2;
				}
		}
	sprite_set_image(treasure, treasure->bitmap);
	sprite_turn_to(treasure, treasure->dx , 0.0);
	sprite_step(treasure);
}

void jump (void){									// Parabolic jump 
	if (player->y < DISPLAY_HEIGHT + PLAYER_HEIGHT + 4){
		player->dy = -(JUMP * 0.4);
	}	else {
		player->dy= -(JUMP);
	}
}

void fall(void){									// Parablic fall 
	if (player->bitmap == player_right && (player->x < screen_width() - PLAYER_WIDTH)){
			sprite_set_image(player, jump_right);
			player->dx = JUMP/2;
	}
	else if (player->bitmap == player_left && (player->x > 0)){
			sprite_set_image(player, jump_left);			
			player->dx = -(JUMP/2);
	}
	player->dy += GRAVITY;
	catch_treasure();
	stable = create_timer(2000);
}

void on_block (void){								// Changes player movement to appropriate horizontal and vertical displacement whne on a block
	player->dy = 0; 								// Player is static 
	player->dx = block_sprites[act_block]->dx;		// Player moves with block
	if (stable != NULL){
		if (timer_expired(stable)){					// Changes image of player if it is still 
			sprite_set_image(player, player_stable);
		}	
	}
	if ((block_sprites[act_block]->x +block_sprites[act_block]->width) > screen_width() || block_sprites[act_block]->x < 0){	// If block disappears, player dies
		dead = true;
	}
	if (dead){										// Falls through blocks if dead 
		player_die();
		player->dy = GRAVITY;
	}
}

void movement_limits(void){							// Limits movement of treasure and treasure to the visible screen 
	if ((player-> x >= screen_width() - PLAYER_WIDTH) || (player-> x <= 0)){	
		player->dx = 0.0;
	}	
	if (player-> y >= screen_height()){				// Player dies when falls off screen 
		if (!dead){
			player_die();
		}
		place_player(first_row);
		dead = false;
	}
	if ((treasure->x <= 0 )|| (treasure->x >= screen_width() - TREASURE_WIDTH) ){		// Treasure bounces off sides of screen 
		direc = (!direc);
		treasure_movement();
	}
}

void movement (void){								// Control direction and speed of player
	keyCode = get_char(); 
	if (on_block_checker(num_blocks)){
		on_block();		
		}	else {															// PLayer is falling 
		fall();	}
	if (keyCode == 'a' && player->x > 1 && !dead && player->dy == 0){		// Move player left according to specification.
		sprite_set_image(player, player_left);
		sprite_move(player, -1, player->dy);	}
	else if(keyCode == 'd' && !dead && player->dy == 0 && (player->x < (screen_width()- sprite_width(player))-1 ) ){		
		sprite_set_image(player, player_right);								// Move player right according to specification.
		sprite_move(player, 1, player->dy);		}
	else if (keyCode == 'w' && player->dy == 0.0 && !dead){
		jump();		}														// Calls jump function only when not falling (on a block) and alive
	movement_limits();
	sprite_turn_to(player, player->dx, player->dy);
	sprite_step(player);
    if (keyCode == 't' ){													// Pausing the treasure 
		paused = (!paused);	 }   
}

/////////////////////////////////////// Set up Game  ///////////////////////////////////////

void setup(void) {									// Initial set up of items 
	start_time = get_current_time();
	draw_display(start_time);						// Draw the display screen 	
	display_blocks();								// Set up blocks 
	block_speed();
	first_row = first_row_blocks();
	place_player(first_row);						// Set up the player at random block on top row.
	sprite_draw(player);							// Draw the player, blocks and treasure. 
	draw_blocks(num_blocks);						
	setup_treasure();
	show_screen();
}

void process(void) {								// Play one turn of game.
	movement();										// Reassign position to player and treasure depending on keyboard input.
    treasure_movement();
	clear_screen();
    sprite_draw(player);							// Draw the player.
    draw_display(start_time);						// Draw the display.
	move_blocks();									// Draw the blocks 
	sprite_draw(treasure);							// Draw treasure
}

void cleanup(void) {								// Clean up game
	
}

int main(void) {									// Program entry point.
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

	cleanup();

	return 0;
}