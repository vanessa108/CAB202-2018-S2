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
#define BLOCK_SPEED (0.03)

#define LCD_WIDTH (84)
#define LCD_HEIGHT (48)
#define HERO_HEIGHT (8)
#define HERO_WIDTH (5)
#define BLOCK_HEIGHT (3)
#define BLOCK_WIDTH (10)
#define TREASURE_HEIGHT (7)
#define TREASURE_WIDTH (7)

#define CHANCE_EMPTY (0.2)
#define CHANCE_FORBIDDEN (0.2)
/** Game Booleans **/
bool game_over = false;
bool paused = false;
bool toggle = false;
bool movingRight = true;

int block_ctr;
int row[3];
/**Define Sprites **/
Sprite hero;

Sprite blocks[20];

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
};
uint8_t forbidden_img [8] = {
    0b11111111,
    0b11111111,
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

/** Structures **/
typedef struct spritePos_t {
    int top;
    int bottom;
    int left;
    int right;
} spritePos_t;

typedef struct blockStand_t {
    bool isStand; 
    int block_num;
} blockStand_t;

void setupBlocks(void) {
    int num_rows = (LCD_HEIGHT - (TREASURE_HEIGHT + 2))/ (HERO_HEIGHT + 2);
    int num_cols = LCD_WIDTH / (BLOCK_WIDTH + 2);
    block_ctr = 0;
    while (block_ctr < 20) {
        for (int row_num = 0; row_num < num_rows; row_num++) {
            //int safe_ctr = 0;
            //speed = BLOCK_SPEED;
            //speed  = get_speed();
            for (int col_num = 0; col_num < num_cols; col_num++) {
                bool isSafe;
                if ( (double) rand() / RAND_MAX < CHANCE_EMPTY) {
                    continue;
                } else {
                    isSafe = ( (double) rand()/RAND_MAX > CHANCE_FORBIDDEN);
                    int col_width = 12;
                    int row_height = BLOCK_HEIGHT + HERO_HEIGHT + 2;
                    double x = col_num * col_width;
                    double y = row_num * row_height + HERO_HEIGHT;
                    row[row_num] = y;
                    if (isSafe) {
                        sprite_init(&blocks[block_ctr], x, y, 10, BLOCK_HEIGHT, safe_img);
                    } else if (!isSafe) {
                        sprite_init(&blocks[block_ctr], x, y, 10, BLOCK_HEIGHT, forbidden_img);
                    }
                    block_ctr++;
                }
            }
        } break;
    }
}

void setupHero(void) {
    sprite_init(&hero, blocks[0].x, blocks[0].y - 8, 5, 8, hero_img);
}

void setupTreasure(void) {
    sprite_init(&treasure, 0, LCD_HEIGHT-7, 7, 7, treasure_img);
}

void drawBlocks(void) {
    for (int i = 0; i < block_ctr; i++) {
        sprite_draw(&blocks[i]);
    }  
}

void moveBlocks(void) {
    for (int i = 0; i < block_ctr; i++) {
        if (blocks[i].y ==  row[0]){
            blocks[i].x = blocks[i].x + 0.3;
        } else if (blocks[i].y == row[1]){
            blocks[i].x = blocks[i].x - 0.3;
        } else if (blocks[i].y == row[2]) {
            blocks[i].x = blocks[i].x + 0.3;
        }       
    }
}

void updateBlocks(void) {
    for (int i = 0; i < block_ctr; i++) {
        if (blocks[i].x > LCD_WIDTH) {
            blocks[i].x = -BLOCK_WIDTH;
        } else if (blocks[i].x < -BLOCK_WIDTH) {
            blocks[i].x = LCD_WIDTH;
        }
    }
}

void blockFunctions(void) {
    moveBlocks();
    updateBlocks();
}

void moveTreasure(void) {
    if (!toggle) {
        if (movingRight) {
            treasure.x += 0.3;
        } 
        if (!movingRight) {
            treasure.x -= 0.3;
        }     
    } 
}

void checkTreasure(void) {
    if (treasure.x < 0){
        movingRight = true;
    } else if (treasure.x > (LCD_WIDTH - 7)) {
        movingRight = false;
    }
}

void treasureFunctions(void) {
    checkTreasure();
    moveTreasure();
}

void drawSprites(void) {
    clear_screen();
    drawBlocks();
    sprite_draw(&hero);
    sprite_draw(&treasure);
    show_screen();
}

void setupGame(void) { 
    setupBlocks();
    setupHero();
    setupTreasure();
}

// bool pixelLevelCollision(Sprite s1, Sprite s2) { /** Taken from week 5 AMS **/
//     for (int y1 = 0; y1 < s1.height; y1++) {
//         for (int x1 = 0; x1 < x1.width; x1++) {
//             int s1offset = y1
//         }
//     }
// }

spritePos_t spritePos(Sprite s) {
    spritePos_t p;
    p.top = s.y;
    p.bottom = s.y + s.height;
    p.left = s.x;
    p.right = s.x + s.width;
    return p;
}
bool isHeroStanding(void) {
    bool isStand = false;
    for (int i = 0; i < block_ctr; i++) {
       spritePos_t blockPos = spritePos(blocks[i]);
       spritePos_t heroPos = spritePos(hero); 
       if (heroPos.bottom == blockPos.top && heroPos.right <= blockPos.right + 5 && heroPos.left >= blockPos.left - 5) {
            isStand = true;
           //thisBlock.block_num = i;
       }
    }
    return isStand;
}

// void heroFreefall(void) {
//     hero.y += 0.3;
// }


void heroGravity(void) {
    bool heroStand = isHeroStanding();
    if (!heroStand) {
        hero.y += 0.2;
    }
    if (heroStand) {
        int heroBottom = hero.y+8;
        if (heroBottom == row[0]|| heroBottom == row[2]) {
            hero.x += 0.3;
        } else if (heroBottom == row[1]) {
            hero.x -= 0.3;
        }
    }

}
bool moveRight = false;
bool moveLeft = false;
void heroMovement(void) {
    //bool heroStand = isHeroStanding();
    if (BIT_IS_SET(PIND, 0)) {
        if (moveLeft) {
            moveLeft = false;
        } else {
            moveRight = true;
        }
    } 
    if (BIT_IS_SET(PINB, 1)) {
        if (moveRight) {
            moveRight = false;
        } else {
            moveLeft = true;
        }
    }
}

void heroRight(void) {
    if (moveRight) {
        hero.x += 0.4;
    }
}

void heroLeft(void) {
    if (moveLeft) {
        hero.x -= 0.4;
    }
}


void heroFunctions(void) {
    //isHeroStanding();
    heroGravity();
    //heroFreefall();
    heroMovement();
    heroRight();
    heroLeft();
}


void enableInput(void) {
    //CLEAR_BIT(DDRD, 1); //up
    CLEAR_BIT(DDRB, 1); //left
    CLEAR_BIT(DDRB, 0); //right
    //CLEAR_BIT(DDRD, 7); //down 
    //CLEAR_BIT(DDRB, 0); //centre
    CLEAR_BIT(DDRF, 5); //right button (SW3)
    CLEAR_BIT(DDRF, 6);  //left button (SW2)
}

void readButtons (void) {
    if (BIT_IS_SET(PINF, 5)) {
        toggle = !toggle;
    }

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

// void pauseScreen(void) {
//     while (paused) {
//             clear_screen();
//             show_screen();
//             } 
// }

// void pause(void) {

//     if (BIT_IS_SET(PINB, 0)) {
//         if (paused) {
//             paused = !paused;
//             clear_screen();
//             drawSprites();
//             show_screen();
//         } else {
//             paused = !paused;
//             pauseScreen();        
//         }
//     }
// }   


void process(void) {
   readButtons(); 
   //pause();
   heroFunctions();
   blockFunctions();
   treasureFunctions();
   drawSprites();
}




int main(void) {
    teensySetup();
    introScreen();
    setupGame();
    for( ;; ) {
        process();
        _delay_ms(33);
    }

    return 0;
}


