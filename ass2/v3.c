#include <avr/io.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <graphics.h>
#include <lcd.h>
#include <sprite.h>
#include <stdbool.h>
#include <lcd_model.h>
#include <util/delay.h>
#include <cpu_speed.h>
#include <macros.h>

/** Game Constants **/
#define blockSpeed (0.03)

#define LCDWidth (84)
#define LCDHeight (48)
#define heroHeight (8)
#define blockHeight (4)
#define blockWidth (10)
#define treasureHeight (7)
#define treasureWidth (7)

#define CHANCE_EMPTY (0.2)
#define CHANCE_FORBIDDEN (0.2)
/** Game Booleans **/
bool game_over = false;
bool paused = false;

int block_ctr;

/**Define Sprites **/
Sprite hero;

Sprite block[20];

Sprite treasure;


/**Sprite Bitmaps **/ 
uint8_t hero_img [8] = {
    0b01110000,
    0b01110000,
    0b01110000,
    0b00100000,
    0b11111000,
    0b00100000,
    0b01010000,
    0b10001000,
};

uint8_t safe_img [8] = {
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
};
uint8_t forbidden_img [8] = {
    0b11111111,
    0b11111111,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11111111,
    0b11111111,
};

uint8_t treasure_img [7] = {
    0b11111110,
    0b10000010,
    0b10111010,
    0b10010010,
    0b10010010,
    0b10000010,
    0b11111110,
};

void setupHero(void) {
    sprite_init(&hero, 13, 12, 8, 8, hero_img);
    sprite_init(&treasure, 20, 20, 7, 7, treasure_img);
    //sprite_init(&block[0], 15, 15, 10, 4, safe_img);
}

void setupBlocks(void) {
    int num_rows = (LCDHeight - (treasureHeight + 2))/ (blockHeight + heroHeight + 2);
    int num_cols = LCDWidth / (blockWidth + 2);
    //int max_blocks = 
    block_ctr = 0;
    while (block_ctr < 20) {
        for (int row_num = 0; row_num < num_rows; row_num++) {
            int safe_ctr = 0;
            //speed = blockSpeed;
            //speed  = get_speed();
            for (int col_num = 0; col_num < num_cols; col_num++) {
                bool isSafe = true;
                if ((col_num == (num_cols - 1)) && (safe_ctr == 0)) {
                    isSafe = true;
                } else {
                    if ( (double) rand() / RAND_MAX < CHANCE_EMPTY) {
                        continue;
                    }
                    isSafe = ( (double) rand()/ RAND_MAX > CHANCE_FORBIDDEN);
                    safe_ctr++;
                }
                int col_width = 12;
                int row_height = blockHeight + heroHeight + 2;
                double x = col_num * col_width;
                double y = row_num * row_height + heroHeight;
                if (isSafe) {
                sprite_init(&block[block_ctr], x, y, 10, 4,safe_img);
                block_ctr++;
                } else {
                    sprite_init(&block[block_ctr], x, y, 10, 4,forbidden_img);
                    block_ctr++;
                }          

            }
        } break;
    }
}




void drawSprites(void) {
    clear_screen();
    //sprite_draw(&hero);
    for (int i = 0; i < block_ctr; i++) {
        sprite_draw(&block[i]);
    }    
    show_screen();
}

void moveBlocks(void) {
    for (int i = 0; i < block_ctr; i++) {
        block[i].x++;
    }
}


void setupGame(void) { 
    setupBlocks();
    setupHero();
}


void enableInput(void) {
//    CLEAR_BIT(DDRD, 1); //up
//    CLEAR_BIT(DDRB, 1); //left
//    CLEAR_BIT(DDRB, 7); //right
//    CLEAR_BIT(DDRD, 0); //down 
//    CLEAR_BIT(DDRB, 0) //centre
//    CLEAR_BIT(DDRF, 5); //right button (SW3)
   CLEAR_BIT(DDRF, 6);  //left button (SW2)
}

void introScreen(void) {
   clear_screen();
   draw_string(18, 11, "Vanessa Li", BG_COLOUR);
   draw_string(20, 20, "n9966463", FG_COLOUR);
   show_screen();
   while (!BIT_IS_SET(PINF, 6)) {} 
}


void teensySetup(void) {
   set_clock_speed(CPU_8MHz);
   enableInput();
   lcd_init(LCD_DEFAULT_CONTRAST);
   lcd_clear();
}

void process(void) {
   setupGame();
   drawSprites();
}
 

int main(void) {
    teensySetup();
    introScreen();
    for( ;; ) {
        process();
        _delay_ms(33);
    }

    return 0;
}


