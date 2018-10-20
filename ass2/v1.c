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

/** Game Booleans **/
bool game_over = false;
bool paused = false;

/**Define Sprites **/
Sprite hero;

Sprite safe_block;

Sprite forbidden_block;

Sprite treasure;


/**Sprite Bitmaps **/ 
uint8_t hero_image [8] = {
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
/** Block Struct **/
typedef struct block_t {
    sprite_id block_img;
    int block_num;
    int row;
    int column;
    bool safe;
}

block_t blocks[];

void setupHero(void) {
    sprite_init(&hero, 13, 12, 8, 8, hero_image);
    sprite_init(&treasure, 20, 20, 7, 7, treasure_img);
}

void setupBlocks(void) {
    int num_rows;
    int num_cols;
    block_ctr = 0;
    while (block_ctr < 250) {
        for (int row_num = 0)
    }
}

// void setupBlock(void) {
//     sprite_init(&safe_block, 20, 20, 10, 4, safe_img);
//     sprite_init(&forbidden_block, 20, 25, 10, 4, forbidden_img);
// }



void drawSprites(void) {
    clear_screen();
    sprite_draw(&hero);
    sprite_draw(&treasure);
    //sprite_draw(&safe_block);
    //sprite_draw(&forbidden_block);
    show_screen();
}

void setupGame(void) { 
    setupHero();
    setupBlock();
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


