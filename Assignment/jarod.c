#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

// Global parameters
#define STUDENT_NUMBER "n9625607"
#define GAME_NAME "JUMPY BOI!"
#define JUMP_SPEED 0.3
#define ACCEL_GRAV 0.005
#define DELAY 10
#define DISPLAY_SCREEN_HEIGHT 5
#define PLAYER_WIDTH 7
#define PLAYER_HEIGHT 3

#define BLOCK_HEIGHT 2
#define BLOCK_WIDTH 10
#define MAX_BLOCKS 200
#define FORBIDDEN_CHANCE 0.2
#define EMPTY_CHANCE 0.2
#define ROW_MAX_SPEED 0.2

/*
block_t
	Struct for containing data about a block.
Fields:
	sprite_id sprite    The sprite object for this block
	int row             The row number for this block, starting from 0
	int column          The column number for this block, starting from 0
	bool safe           True if safe '=', false if forbidden 'X'
*/
typedef struct block_t {
	sprite_id sprite;
	int row;
	int column;
	bool safe;
} block_t;

// Global variables
bool game_over = false;    // Game loops forever until this is set to true
int lives = 10;            // Number of lives, default 10
int score = 0;             // Game score, default 0
int time_zero;             // Epoch time at game launch
int width;                 // Screen width, fixed throughout the game
int height;                // Screen height, fixed throughout the game
int keyCode;               // Current key pressed

int row_height;            // Height of each block row
int col_width;             // Width of each block column
int num_rows;              // Number of block rows
int num_cols;              // Number of block columns
int num_blocks;            // Number of blocks in total
block_t block_array[MAX_BLOCKS];  // Array for containing block_t objects

sprite_id player;          // Player sprite_id

// Sprites
char *player_right =
	" (\\ (\\ "
	"( >.> )"
	"(\") (\")";
char *player_left =
	" /) /) "
	"( <.< )"
	"(\") (\")";
char *player_down =
	" /\\ /\\ "
	"( -.- )"
	"(\") (\")";
char *player_up =
	" /\\ /\\ "
	"( ^.^ )"
	"(\") (\")";
char *safe_blocks[] = {
	"   =====  "
	"   =====  ",
	"  ======  "
	"  ======  ",
	"  ======= "
	"  ======= ",
	" ======== "
	" ======== ",
	" ========="
	" =========",
	"=========="
	"=========="
};
char *forbidden_blocks[] = {
	"   XXXXX  "
	"   XXXXX  ",
	"  XXXXXX  "
	"  XXXXXX  ",
	"  XXXXXXX "
	"  XXXXXXX ",
	" XXXXXXXX "
	" XXXXXXXX ",
	" XXXXXXXXX"
	" XXXXXXXXX",
	"XXXXXXXXXX"
	"XXXXXXXXXX"
};
char *treasure[] = {
	" /\\ "
	"/_\\ "
	" VV "
}

/*
draw_display_screen (void)
	Draws the display screen at the top of the screen, visible at all times. 
	Contains student number, lives remaining, playing time formatted as mm:ss,
	score and border. Five characters high.
*/	
void draw_display_screen() {
	// Draw the border above and below
	draw_line(1, 0, width, 0, '~');
	draw_line(1, DISPLAY_SCREEN_HEIGHT-1, width, DISPLAY_SCREEN_HEIGHT-1, '~');
	
	// Find the time elapsed and split into minutes and seconds
	int time_elapsed = get_current_time() - time_zero;
	int minutes = floor(time_elapsed / 60);
	int seconds = floor(time_elapsed % 60);
	
	// Draw the formatted display screen string
	draw_formatted(1, round(DISPLAY_SCREEN_HEIGHT/2), 
		"  "GAME_NAME"  "STUDENT_NUMBER"  \
[Lives: %02d]  [Time: %02d:%02d]  [Score: %d] [num_blocks: %d]",
		lives, minutes, seconds, score, num_blocks);
}

/*
setup_blocks (void)
	Sets up blocks randomly with no observable pattern. All blocks are added to
	the block_t array block_array
*/
void setup_blocks() {
	// Row and column parameters
	row_height = PLAYER_HEIGHT + 2 + BLOCK_HEIGHT;
	col_width = BLOCK_WIDTH + 1;
	num_rows = floor((height - DISPLAY_SCREEN_HEIGHT) / row_height);
	num_cols = floor(width / col_width);
	
	// Populate block_array, stopping at max block count
	num_blocks = 0;
	while (num_blocks < MAX_BLOCKS) {
		
		// Loop over all the rows and columns
		for (int curr_row = 0; curr_row < num_rows; curr_row++) {
			int safe_count = 0;
			
			// Calculate a random speed for this row
			double speed = 0;
			if (curr_row > 0 && curr_row < (num_rows-1)) {
				speed = ((double)rand()/RAND_MAX-0.5) *2*ROW_MAX_SPEED;
			}
			
			for (int curr_col = 0; curr_col < num_cols; curr_col++) {
				
				// Force the block to be safe if there are none in the row
				bool safe = true;
				if (curr_col == num_cols-1 && safe_count <= 0) {
					safe = true;
				} else {
					// Randomly make this block empty
					if ((double) rand() / (RAND_MAX) < EMPTY_CHANCE) {
						continue;
					}
					// Randomly make this block forbidden
					safe = ((double) rand() / (RAND_MAX) > FORBIDDEN_CHANCE);
					safe_count += safe;
				}
				
				// Calculate some parameters
				int x = curr_col * col_width;
				int y = (curr_row+1) * row_height + DISPLAY_SCREEN_HEIGHT-1;
				int block_width = rand() % 5;    // index of the block sprite
				char *sprite = safe ?
				               safe_blocks[block_width] : 
				               forbidden_blocks[block_width];
				
				// Apply these parameters to the block in block_array
				block_array[num_blocks].row = curr_row;
				block_array[num_blocks].column = curr_col;
				block_array[num_blocks].safe = safe;
				block_array[num_blocks].sprite = sprite_create(x, y, 
				                             BLOCK_WIDTH, BLOCK_HEIGHT, sprite);
				
				// Make this block move
				sprite_turn_to(block_array[num_blocks].sprite, speed, 0);
				
				num_blocks++;
			}
		}
		break;
	}
}

/*
update_blocks (void)
	Draws all blocks and updates their positions
*/
void update_blocks() {
	for (int i = 0; i < num_blocks; i++) {
		sprite_id *curr_sprite = &block_array[i].sprite;
		sprite_step(*curr_sprite);
		
		int x = round(sprite_x(*curr_sprite));
		int y = round(sprite_y(*curr_sprite));
		
		// Wrap around the screen
		if (x < -BLOCK_WIDTH) {
			sprite_move_to(*curr_sprite, width, y);
		} else if (x > width) {
			sprite_move_to(*curr_sprite, -BLOCK_WIDTH, y);
		}
		
		sprite_draw(*curr_sprite);
	} 
}

/*
update_player (void)
	Controls player movement including gravity
*/
void update_player() {
	sprite_step(player);
	
	// Get the position and velocity of the player
	int px = round(sprite_x(player));
	//int py = round(sprite_y(player));
	double pdx = sprite_dx(player);
	double pdy = sprite_dy(player);
	
	// Move hero left according to specification.
	if (keyCode == 'a' && px > 1) {
		sprite_move(player, -1, 0);
		sprite_set_image(player, player_left);
	}

	// Move hero right according to specification.
	else if (keyCode == 'd' && px < width-1-sprite_width(player)) {
		sprite_move(player, 1, 0);
		sprite_set_image(player, player_right);
	}
	
	// Up/down movement
	if (keyCode == 'w') {
		pdy = -JUMP_SPEED;
		sprite_set_image(player, player_up);
	} else {
		// Gravity!
		pdy += ACCEL_GRAV;
	}
	if (pdy > ACCEL_GRAV) {
		sprite_set_image(player, player_down);
	}
	
	// Move the sprite with new velocity
	sprite_turn_to(player, pdx, pdy);
	sprite_draw(player);
}

/*
setup (void)
	Run once when the game starts to set up variables and other things.
*/
void setup() {
	// Get screen height and width, this stays constant throughout the game
	width = screen_width();
	height = screen_height();
	
	// Store the start of game Unix epoch time
	time_zero = get_current_time();
	
	// Setup up the player on the screen
	player = sprite_create(5, 10, PLAYER_WIDTH, PLAYER_HEIGHT, player_right);
	sprite_draw(player);
	
	// Setup blocks
	setup_blocks();
	
}

/*
process (void)
	Run repeatedly until game_over is set to true. Main function of the game.
*/
void process() {
	// Update current key code
	keyCode = get_char();
	
	draw_display_screen();
	update_player();
	update_blocks();
    sprite_step(treasure);
}

/*
main (void)
	The main C function.
*/
int main(void) {
	// Setup game
	setup_screen();
	setup();
	show_screen();
	
	// Loop until game over
	while (!game_over) {
		clear_screen();
		process();
		show_screen();
		timer_pause(DELAY);
	}
	
	return 0;
}