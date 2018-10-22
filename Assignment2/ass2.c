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
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h> 

#include "usb_serial.h"

/** Game Constants **/
#define BLOCK_SPEED (0.03)

#define LCD_WIDTH (84)
#define LCD_HEIGHT (48)
#define HERO_HEIGHT (8)
#define HERO_WIDTH (6)
#define BLOCK_HEIGHT (2)
#define BLOCK_WIDTH (10)
#define TREASURE_HEIGHT (7)
#define TREASURE_WIDTH (7)
#define ZOMBIE_WIDTH (5)
#define ZOMBIE_HEIGHT (7)
#define FOOD_HEIGHT (2)
#define FOOD_WIDTH (2)

#define CHANCE_EMPTY (0.35)
#define CHANCE_FORBIDDEN (0.2)

//PWM FROM EXAMPLE
#define BIT(x) (1 << (x))
#define OVERFLOW_TOP (1023)

//BUTTON DEBOUNCE VARIABLES
static uint8_t upPrevState = 0;
static uint8_t downPrevState = 0;
static uint8_t leftPrevState = 0;
static uint8_t rightPrevState = 0;
static uint8_t centrePrevState = 0;
static uint8_t SW2PrevState = 0;
static uint8_t SW3PrevState = 0;

volatile uint8_t up_ctr = 0;
volatile uint8_t up_cls = 0;
volatile uint8_t down_ctr = 0;
volatile uint8_t down_cls = 0;
volatile uint8_t left_ctr = 0;
volatile uint8_t left_cls = 0;
volatile uint8_t right_ctr = 0;
volatile uint8_t right_cls = 0;
volatile uint8_t centre_ctr = 0;
volatile uint8_t centre_cls = 0;
volatile uint8_t SW2_ctr = 0;
volatile uint8_t SW2_cls = 0;
volatile uint8_t SW3_ctr = 0;
volatile uint8_t SW3_cls = 0;

bool downPrevious = false;
bool centrePrevious = false;
bool SW3Previous = false;

bool upPress;
bool downPress;
bool leftPress;
bool rightPress;
bool centrePress;
bool SW2Press;
bool SW3Press;

/** Game Booleans **/
bool game_over = false;
bool paused = false;
bool wait = false;
bool toggle = false;
bool end_game = false;

/** Hero Movement **/
bool moveRight = false;
bool moveLeft = false;

/** Pause display **/
int lives = 10;
int score = 0;
int foodCount = 5;
int zombieCount = 5;

/** Block variables **/
int block_ctr = 0;
bool movingRight = true;
double block_speed;

int previousBlock;
int currentBlock;

/** Specalised respawn animations **/
/** This is a comment **/
int backlight = 800;
bool respawn;
int LCD_Contrast = LCD_DEFAULT_CONTRAST;
int contrast_ctr;
/**Zombie spawn variables **/
double previousDrop = 0;
bool zombiesExist;
bool zombiesFalling;
int fallingMessage_ctr = 0;

/** Misc variables **/
int numberOfZombiesFed = 0;
uint8_t direct_y = 8; //for end animation 
bool zright[5] = {1, 1, 1, 1, 1}; //zombie moving right
int q; //for usb serial input
int endTime; //for game over screen
int time_at_pause;
int pause_elapsed;
/** Define Sprites **/
Sprite hero;

Sprite blocks[20];

Sprite treasure;

Sprite food[5];

Sprite zombie[5];

/**Sprite Bitmaps **/ 
uint8_t hero_img [8] = {
    0b00000100,
    0b01110100,
    0b01110100,
    0b00100100,
    0b11111000,
    0b00100000,
    0b01010000,
    0b10001000,
};

uint8_t safe_img [4] = {
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
};
uint8_t forbidden_img [4] = {
    0b10101010,
    0b10100000,
    0b10101010,
    0b10111111,
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

uint8_t food_img[2] = {
    0b11000000,
    0b11000000,
};

uint8_t zombie_img[7] = {
    0b10001000,
    0b10101000,
    0b11111000,
    0b00100000,
    0b00100000,
    0b01010000,
    0b10001000,
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

/** #####################################
 *  ADC FROM adc example
 * ####################################**/

void adc_init() {
    //ADC Enable and prescaler of 128
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read(uint8_t channel) {
    ADMUX = (channel & ((1 << 5) - 1)) | (1 << REFS0);
    ADCSRB = (channel & (1 << 5));

    ADCSRA |= (1 << ADSC);

    while ( ADCSRA & (1 << ADSC)) {}
    return ADC;
}

/** #####################################
 *  SERIAL COMMMUNICATION FROM usb_sprite example
 * ####################################**/

void usb_serial_send(char * message) {
	// Cast to avoid "error: pointer targets in passing argument 1 
	//	of 'usb_serial_write' differ in signedness"
	usb_serial_write((uint8_t *) message, strlen(message));
}

/*
**	Transmits an integer value via usb_serial
*/

void usb_serial_send_int(int value) {
	static char buffer[8];
	snprintf(buffer, sizeof(buffer), "%d", value);
	usb_serial_send( buffer );
}


/**###############
 * Direct draw LED and Char (code from topic 8 example 3)
 * ############################### **/


void draw_char_direct(int x, int y, char ch, colour_t colour) {
    // Do nothing if character does not fit on LCD.
    if (x < 0 || x > LCD_X - CHAR_WIDTH || y < 0 || y > LCD_Y - CHAR_HEIGHT) {
        return;
    }

    // Move LCD cursor to starting spot.
    LCD_CMD(lcd_set_function, lcd_instr_basic | lcd_addr_horizontal);
    LCD_CMD(lcd_set_x_addr, ( x & 0x7f));
    LCD_CMD(lcd_set_y_addr, (y & 0x7f) / 8);

    // Send pixel blocks.
    for (int i = 0; i < CHAR_WIDTH; i++) {
        uint8_t pixelBlock = pgm_read_byte(&(ASCII[ch - ' '][i]));

        if (colour == BG_COLOUR) {
            pixelBlock = ~pixelBlock;
        }

        LCD_DATA(pixelBlock);
    }
}


void erase_char_direct(int x, int y, char ch, colour_t colour) {
    // Do nothing if character does not fit on LCD.
    if (x < 0 || x > LCD_X - CHAR_WIDTH || y < 0 || y > LCD_Y - CHAR_HEIGHT) {
        return;
    }

    // Move LCD cursor to starting spot.
    LCD_CMD(lcd_set_function, lcd_instr_basic | lcd_addr_horizontal);
    LCD_CMD(lcd_set_x_addr, ( x & 0x7f));
    LCD_CMD(lcd_set_y_addr, (y & 0x7f) / 8);

    // Send pixel blocks.
    for (int i = 0; i < CHAR_WIDTH; i++) {
        uint8_t pixelBlock = pgm_read_byte(&(ASCII[ch - ' '][i]));

        if (colour == BG_COLOUR) {
            pixelBlock = ~pixelBlock;
        }

        LCD_DATA(0);
    }
}



void draw_string_direct(int x, int y, char * str, colour_t colour) {
    for (int i = 0; str[i] != 0; i++, x += CHAR_WIDTH) {
        draw_char_direct(x, y, str[i], colour);
    }
}

void erase_string_direct(int x, int y, char * str, colour_t colour) {
    for (int i = 0; str[i] != 0; i++, x += CHAR_WIDTH) {
        erase_char_direct(x, y, str[i], colour);
    }
}



void direct_message(void) {
    erase_string_direct(20, direct_y, "Game Over", FG_COLOUR);
    direct_y = (direct_y+1) % (LCD_WIDTH - 8);
    draw_string_direct(20, direct_y, "Game Over", FG_COLOUR);
}

/** #####################################
 *  GAME TIMER (code from ams 9 ex 2)
 * ####################################**/

volatile uint32_t time_ctr = 0;
//interrup service 
ISR(TIMER1_OVF_vect) {
    time_ctr++;
}

double current_time(void) {
    return (time_ctr * 65536.0 + TCNT1) * 64 / 8e6;
}

/* SETUP FUNCTION */
int row[3] = {21, 33, 45};
void setupBlocks(void) {
    int num_rows = 3;
    int num_cols = LCD_WIDTH / (BLOCK_WIDTH + 2);
    while (block_ctr < 20) {
        for (int row_num = 0; row_num < num_rows; row_num++) {
            srand(TCNT1);
            for (int col_num = 0; col_num < num_cols; col_num++) {
                bool isSafe;
                if ( (double) rand() / RAND_MAX < CHANCE_EMPTY) {
                    continue;
                } else {
                    isSafe = ( (double) rand()/RAND_MAX > CHANCE_FORBIDDEN);
                    int col_width = 12;
                    //int row_height = BLOCK_HEIGHT + HERO_HEIGHT + 3;
                    double x = col_num * col_width;
                    double y = row[row_num];
                    if (isSafe) {
                        sprite_init(&blocks[block_ctr], x, y, BLOCK_WIDTH, BLOCK_HEIGHT, safe_img);
                    } else if (!isSafe) {
                        sprite_init(&blocks[block_ctr], x, y, BLOCK_WIDTH, BLOCK_HEIGHT, forbidden_img);
                    }
                    block_ctr++;
                }
            }
        } break;
    }
}

void startBlock (void) {
    sprite_init(&blocks[block_ctr], 74, 9, BLOCK_WIDTH, BLOCK_HEIGHT, safe_img); 
    block_ctr++;
}



void setupHero(void) {
    sprite_init(&hero, blocks[block_ctr-1].x , blocks[block_ctr-1].y - HERO_HEIGHT, HERO_WIDTH, HERO_HEIGHT, hero_img);
}

void setupTreasure(void) {
    sprite_init(&treasure, 0, 38, 7, 7, treasure_img);
}

void setupFood(void) {
    for (int i = 0; i < 5; i++) {
        sprite_init(&food[i], 81, 0, 2, 2, food_img);
        food[i].is_visible = 0;
    }
}

void drawFood(void) {
    for (int i = 0; i < 5; i++) {
        sprite_draw(&food[i]);
    }
}

int zombie_start[5] = {5, 19, 31, 53, 69};
void setupZombies(void) {
    for (int i = 0; i < 5; i++) {
        sprite_init(&zombie[i], zombie_start[i], -7, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, zombie_img);
    }
}

void drawZombies(void) {
    for (int i = 0; i < 5; i++) {
        sprite_draw(&zombie[i]);
    }
}

/* Sprite Setup Functions */

void drawBlocks(void) {
    for (int i = 0; i < block_ctr; i++) {
        sprite_draw(&blocks[i]);
    }  
}

void moveBlocks(void) {
    double rightPot = adc_read(1);
    if (rightPot < 1) {
        block_speed = 0;
    } else if (rightPot > 1 && rightPot < 256) {
        block_speed = 0.1;
    } else if (rightPot > 256 && rightPot < 512) {
        block_speed = 0.3;
    } else if (rightPot > 512 && rightPot < 768) {
        block_speed = 0.5;
    } else if (rightPot > 768 && rightPot < 1024) {
        block_speed = 0.7;
    }
    for (int i = 0; i < block_ctr; i++) {
        if (blocks[i].y ==  row[0] || blocks[i].y == row[2]){
            blocks[i].dx = block_speed;
        } else if (blocks[i].y == row[1]){
            blocks[i].dx = -block_speed;      
        }
        blocks[i].x += blocks[i].dx;
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
            treasure.dx = 0.5;
        } 
        if (!movingRight) {
            treasure.dx = -0.5;
        }  
        treasure.x += treasure.dx;  
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
    drawFood();
    drawZombies();
    show_screen();
}

void playerPositionMessage(void) {
    usb_serial_send("\r\nPlayer position: (");
	usb_serial_send_int((int)hero.x);
	usb_serial_putchar(',');
	usb_serial_send_int((int)hero.y);
	usb_serial_send(")\r\n");
}
void startSerialMessage(void) {
    usb_serial_send("\nGame has started");
    playerPositionMessage();
}


void setupGame(void) {
    setupBlocks();
    startBlock();
    setupHero();
    setupTreasure();
    setupFood();
    setupZombies();
    startSerialMessage();
    previousBlock = block_ctr-1;
}

/** Game Mechanics **/

void gameTimeMessage(void) {
    int now = current_time();
    int minutes = floor(now/60);
    int seconds = floor(now % 60);
    usb_serial_send("\r\n Time: "); usb_serial_send_int((int) minutes); usb_serial_send(":"); usb_serial_send_int((int) seconds);
}

void livesMessage(void) {
    usb_serial_send("\r\n Lives remaining: "); usb_serial_send_int((int)lives);
}

void scoreMessage(void) {
    usb_serial_send("\r\n Current score: "); usb_serial_send_int((int) score); 
}

void playerDeathMessage(char * reason) {
    
    usb_serial_send("\n\rPlayer has died");
    usb_serial_send("\r\n Cause of death: "); usb_serial_send(reason);
    livesMessage();
    scoreMessage();
    gameTimeMessage();
}

void playerTreasureMessage(void) {
    usb_serial_send("\n\rPlayer collided with treasure");
    scoreMessage();
    livesMessage();
    gameTimeMessage();
    playerPositionMessage();
}

void playerRespawnMessage(void) {
    usb_serial_send("\n\rPlayer respawned");
    playerPositionMessage();
}

void pauseMessage(void) {
    usb_serial_send("\nGame Paused");
    livesMessage();
    scoreMessage();
    gameTimeMessage();
    usb_serial_send("\r\nNumber of zombies: "); usb_serial_send_int((int) zombieCount);
    usb_serial_send("\r\nFood in inventory: "); usb_serial_send_int((int) foodCount);
}


void specialisedRespawn(void) {
    if (respawn) {
        if (contrast_ctr < 10) {
            LCD_Contrast -= 3;
            backlight -= 80;
            contrast_ctr++;
        } else if (contrast_ctr >= 10 && contrast_ctr < 20 ) {
            LCD_Contrast += 3;
            backlight += 80;
            contrast_ctr++;
        } else if (contrast_ctr == 20) {
            LCD_Contrast = LCD_DEFAULT_CONTRAST;
            backlight = 800;
            contrast_ctr = 0;
            respawn = false;
        }
    }
}

void heroRespawn(void) {
    free(&hero);
    if (lives > 0) {
        respawn = true;
    }
    setupHero();
    moveRight = false;
    moveLeft = false;
    

}

spritePos_t spritePos(Sprite s) {
    spritePos_t p;
    p.top = s.y;
    p.bottom = s.y + s.height;
    p.left = s.x;
    p.right = s.x + s.width - 1;
    return p;
}

bool pixel_level_collision(Sprite h, Sprite b) {
    for (int x_h = 0; x_h < HERO_WIDTH; x_h++) {
        if (((h.bitmap[1] >> (7- x_h)) & 1) != 0) {
            int h_solidx = h.x + x_h;
            for (int x_b = 0; x_b < 2; x_b++) {  
                if ( ((b.bitmap[(int) 5+ x_b/8] >> (7- x_b % 8)) & 1) != 0) {
                    int b_solidx = b.x + x_b;
                    if (b_solidx == h_solidx - 2 || b_solidx == h_solidx+2) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool isHeroHolding(void) {
    for (int i = 0; i < block_ctr; i++) {
       spritePos_t blockPos = spritePos(blocks[i]);
       spritePos_t heroPos = spritePos(hero); 
       if (heroPos.top == blockPos.bottom  && heroPos.right <= blockPos.right + 5 && heroPos.left >= blockPos.left - 5) {
           if (pixel_level_collision( hero, blocks[i]) ) {
               return true;
           }
       } 
    }
    return false;
}

bool isHeroStanding(void) {
    bool isStand = false;
    for (int i = 0; i < block_ctr; i++) {
       spritePos_t blockPos = spritePos(blocks[i]);
       spritePos_t heroPos = spritePos(hero); 
       if (heroPos.bottom == blockPos.top  && heroPos.right <= blockPos.right + 5 && heroPos.left >= blockPos.left - 5) {
            if (blocks[i].bitmap == safe_img) {
                isStand = true;
                currentBlock = i;
                hero.y = blockPos.top - HERO_HEIGHT;
            } else {
                lives -= 1;
                playerDeathMessage("Forbidden block");
                heroRespawn();
            }
       }
    }
    return isStand;
}



bool isHeroCollidingSide(void) {
    bool isCollide = false; 
    for (int i = 0; i < block_ctr; i++) {
        spritePos_t b = spritePos(blocks[i]);
        spritePos_t h = spritePos(hero);
        if ((h.left >= b.right + 3 || h.right >= b.left - 3) && 
        ((h.top < b.top && h.bottom > b.top) || 
        (h.bottom > b.bottom && h.top < b.bottom))) {
            isCollide = true;
        }
    }
    return isCollide;
}

void heroOffscreen(void) {
    spritePos_t heroPos = spritePos(hero);
    if (heroPos.bottom > LCD_HEIGHT || heroPos.top < 0 ||
    heroPos.left < 0 || heroPos.right > LCD_WIDTH) {
        lives -= 1;
        playerDeathMessage("Player is offscreen");
        heroRespawn();
    }

}
void scoreOnBlock(void) {
    bool heroStand = isHeroStanding();
    if (heroStand && currentBlock != previousBlock){
        previousBlock = currentBlock;
        score++;
    }
}

void heroControls(void) {
        if (rightPress) {
            if (moveLeft) {
                moveLeft = false;
            } else {
                moveRight = true;
            }
        } 
        if (leftPress) {
            if (moveRight) {
                moveRight = false;
            } else {
                moveLeft = true;
            }
        }
}

void heroMovement(void) {
    bool heroStand = isHeroStanding();
    bool heroCollide = isHeroCollidingSide();
    if (heroCollide) {
        if (hero.dx > 0) {
            hero.dx = 0;
        } else if (hero.dx < 0) {
            hero.dx = 0;
        }
    }
    if (upPress) {
        if (heroStand) {
        hero.dy = -2.3;
        hero.y += hero.dy;
        }   
    }
    if (heroStand) {
        int heroBottom = hero.y+ HERO_HEIGHT;
        if (heroBottom == row[0]|| heroBottom == row[2]) {
            hero.dx = block_speed;
        } else if (heroBottom == row[1]) {
            hero.dx = -block_speed;
        }
    }
    if (moveRight) {
        if (hero.y + HERO_HEIGHT == row[1]) {
            hero.dx = block_speed + 0.5;
        } else {
            hero.dx = block_speed + 0.5;
        }
    } else if (moveLeft) {
        if (hero.y + HERO_HEIGHT == row[1]) {
            hero.dx = -(block_speed + 0.5);
        } else {
            hero.dx = -(block_speed + 0.5);
        }
    }
    hero.x += hero.dx;
}

void heroGravity(void) {
    bool heroStand = isHeroStanding();
    bool heroHold = isHeroHolding();
    if (!heroStand) {
        hero.dy += 0.2;
        if (hero.dy > 1) {
            hero.dy = 1;
        }
        moveRight = false;
        moveLeft = false;
    }
    if (heroStand || heroHold) {
        hero.dy = 0;
    }
    hero.y += hero.dy;
}

void heroTreasure(void) {
    spritePos_t t = spritePos(treasure);
    spritePos_t h = spritePos(hero);
    if (h.bottom >= t.top && h.left >= t.left - 3 && h.right <= t.right + 3) {
        lives += 2;
        treasure.is_visible = 0;
        free(&treasure);
        heroRespawn();
        playerTreasureMessage();
    }
}

void zombieHeroCollision(void) {
    for (int i = 0; i < 5; i++) {
        spritePos_t h = spritePos(hero);
        spritePos_t z = spritePos(zombie[i]);
        if (((h.left <= z.left && h.right >= z.left) || (h.right >= z.right && h.left <= z.right))
        && (h.top <=  z.top && h.bottom >= z.top ) ) {
            lives -= 1;
            playerDeathMessage("Player collided with zombie");
            heroRespawn();
        }
    }
}




void heroFunctions(void) {
    if (!respawn) {
        heroControls();
        heroMovement();
    }
    heroGravity();;
    heroOffscreen();
    heroTreasure();
    scoreOnBlock();
    //zombieHeroCollision();
    specialisedRespawn();
}

/** Zombie and Food **/
bool isZombieStanding(Sprite zomb) {
    bool isStand = false;
    for (int i = 0; i < block_ctr; i++) {
        spritePos_t z = spritePos(zomb);
        spritePos_t b = spritePos(blocks[i]);
        if (z.bottom == b.top && z.left <= b.right && z.right >= b.left) {
            isStand = true;
        }
    }
    return isStand;
}

void zombieMessage(void) {
    usb_serial_send("\r\nZombies spawning!");
    usb_serial_send("\r\nNumber of zombies: "); usb_serial_send_int((int) zombieCount);
    gameTimeMessage();
    livesMessage();
    scoreMessage();
}

void zombieFoodMessage(void) {
    usb_serial_send("\r\nZombie collided with food");
    usb_serial_send("\r\nNumber of zombies: "); usb_serial_send_int((int) zombieCount);
    usb_serial_send("\r\nFood in inventory: "); usb_serial_send_int((int) foodCount);
    gameTimeMessage();
}



volatile uint32_t timer3_ctr = 0;
ISR(TIMER3_OVF_vect) {
    timer3_ctr++;
}


void turnOnLEDS(void) {
    SET_BIT(PORTB, 2);
    SET_BIT(PORTB, 3);
}

void clearLEDS(void) {
    CLEAR_BIT(PORTB, 2);
    CLEAR_BIT(PORTB, 3);
}


void flashingLEDS(void) {
    if (timer3_ctr >= 4) {
        turnOnLEDS();
        timer3_ctr = 0;
    } else {
        clearLEDS();
    }
}



void zombieTimer(void) {
    if (zombieCount <= 0) {
        fallingMessage_ctr = 0;
        previousDrop = current_time();
        zombieCount = 5;
        for (int i = 0; i < 5; i++) {
            zombie[i].dx = 0;
            zombie[i].x = zombie_start[i];
        }
    }
    int dropTime = current_time() - floor(previousDrop);
    if (dropTime >= 3) {
        zombiesExist = true;
    } else {
        zombiesExist = false;
    }

    if (dropTime >= 3 && dropTime <= 10) {
        zombiesFalling = true;
        fallingMessage_ctr++;
    } else {
        zombiesFalling = false;
    }
}



void zombieGravity(void) {
    for (int i = 0; i < 5; i++) {
        bool zombieStand = isZombieStanding(zombie[i]);
        if (!zombieStand && zombie[i].dx == 0) {
            zombie[i].dy += 0.3;
            zombie[i].dx = 0;
            if (zombie[i].dy > 1) {
                zombie[i].dy = 1;
            }
        } else if (zombieStand) {
            zombie[i].dy = 0;
            int zombieBot = zombie[i].y + ZOMBIE_HEIGHT;
            if (zombieBot >= row[0] && zombieBot <= row[0]+BLOCK_HEIGHT) {
                zombie[i].y = row[0] - ZOMBIE_HEIGHT;
            } else if (zombieBot >= row[1] && zombieBot <= row[1]+BLOCK_HEIGHT) {
                zombie[i].y = row[1] - ZOMBIE_HEIGHT;
            } else if (zombieBot >= row[2] && zombieBot <= row[2]+BLOCK_HEIGHT) {
                zombie[i].y = row[2] - ZOMBIE_HEIGHT;
            } 
        }
        zombie[i].y += zombie[i].dy;
    }
}

void zombieMovement(void) {
    double zombieSpeed = block_speed + 0.3; 
    for (int i = 0; i < 5; i++) {
        bool zombieStand = isZombieStanding(zombie[i]);
        if (!zombieStand && zombie[i].dx != 0) {
            if (zombie[i].dx > 0) {
                zombie[i].x -= 2; 
                zright[i] = 0;
            }
            if (zombie[i].dx < 0) {
                zombie[i].x += 2;
                zright[i] = 1;
            }
        } else if (zombieStand) {
            if (zright[i]) {
                zombie[i].dx = zombieSpeed;
            } else if (!zright[i]) {
                zombie[i].dx = -zombieSpeed;
            }
        }
        zombie[i].x += zombie[i].dx;
    }
}


void zombieWrap(void) {
    for (int i = 0; i < 5; i++) {
        if (zombie[i].x > LCD_WIDTH) {
            zombie[i].x = -ZOMBIE_WIDTH;
        } else if (zombie[i].x < -ZOMBIE_WIDTH) {
            zombie[i].x = LCD_WIDTH;
        }        
    }
}



void zombieFoodCollision(void) {
    for (int i = 0; i < 5; i++) {
        spritePos_t z = spritePos(zombie[i]);
        for (int j = 0; j < foodCount; j++) {
            spritePos_t f = spritePos(food[j]);
            if (z.bottom == f.bottom && ((z.right >= f.right && z.left <= f.left) ||
            (z.left <= f.left && z.right >= f.left))) {
                food[j].x = 81;
                food[j].y = 0;
                food[j].is_visible = 0;
                foodCount++;
                zombie[i].y = -ZOMBIE_HEIGHT;
                zombieCount--;
                lives += 10;
                zombieFoodMessage();
                numberOfZombiesFed++;
            }
        }

    }
}

void zombieOffscreen(void) {
    for (int i = 0; i < 5; i++) {
        if (zombie[i].y > LCD_HEIGHT) {
            zombieCount--;
            zombie[i].y = -7;
            zombie[i].dy = 0;
            zombie[i].dx = 0.2;
        }
    }
}


void zombieFunctions(void) { 
    zombieTimer();
    if (zombiesExist) {
        zombieGravity();
    } 
    if (zombiesFalling) {
        flashingLEDS();
    } else if (!zombiesFalling) {
        clearLEDS();
    }
    if (fallingMessage_ctr == 1) {
        zombieMessage();
    }
    zombieOffscreen();
    zombieMovement();
    zombieFoodCollision();
    zombieWrap();
}



/* #############
FOOD FUNCTIONS
################*/

void dropFood(void) {
    bool heroStand = isHeroStanding();
    int availableFood[5];
    for (int i = 0; i < 5; i++) {
        if (food[i].is_visible == 0) {
            availableFood[i] = i;
        }
    }
    int fc = availableFood[5 - foodCount];
    if (downPress && foodCount > 0 && heroStand) {
        food[fc].x = hero.x + 2;
        food[fc].y = hero.y + HERO_HEIGHT - 2;
        food[fc].is_visible = 1;
        foodCount -= 1;
    }
}

void foodOnBlock(void) {
    for (int i = 0; i < 5; i++) {
        spritePos_t f = spritePos(food[i]);
        for (int j = 0; j < block_ctr; j++) {
            spritePos_t b = spritePos(blocks[j]);
            if (f.bottom == b.top  && f.right <= b.right && f.left >= b.left) {
                food[i].dx = blocks[j].dx;
            }
        }
        food[i].x += food[i].dx;
    }
}

void foodWrap(void) {
    for (int i = 0; i < 5; i++) {
        if (food[i].x > LCD_WIDTH + 6) {
            food[i].x = -(FOOD_WIDTH);
        } else if (food[i].x < -FOOD_WIDTH - 6) {
            food[i].x = LCD_WIDTH;
        }        
    }
}


void foodFunctions(void) {
    dropFood();
    foodOnBlock();
    foodWrap();
}


/** #####################################
 * DEBOUNCE AND READ BUTTONS
 * ####################################**/

void enableInput(void) {
    SET_INPUT(DDRD, 1); //up
    SET_INPUT(DDRB, 1); //left
    SET_INPUT(DDRD, 0); //right
    SET_INPUT(DDRB, 7); //down 
    SET_INPUT(DDRB, 0); //centre
    SET_INPUT(DDRF, 5); //right button (SW3)
    SET_INPUT(DDRF, 6);  //left button (SW2)
}


// Using code from AMS 9 ex3
ISR(TIMER0_OVF_vect) {

    up_ctr = ((up_ctr << 1) & 0b00011111) | (BIT_IS_SET(PIND, 1) || q == 'w');
    if (up_ctr == 0b00011111) {
        up_cls = 1;
    } else if (up_ctr == 0) {
        up_cls = 0;
    }

    down_ctr = ((down_ctr << 1) & 0b00011111) | (BIT_IS_SET(PINB, 7) || q == 's');
    if (down_ctr == 0b00011111) {
        down_cls = 1;
    } else if (down_ctr == 0) {
        down_cls = 0;
    }
    if (down_cls != downPrevious) {
        downPrevious = down_cls;
    }

    left_ctr = ((left_ctr << 1) & 0b00011111) | (BIT_IS_SET(PINB, 1) || q == 'a');
    if (left_ctr == 0b00011111) {
        left_cls = 1;
    } else if (left_ctr == 0) {
        left_cls = 0;
    }

    right_ctr = ((right_ctr << 1) & 0b00011111) | (BIT_IS_SET(PIND, 0) || q == 'd');
    if (right_ctr == 0b00011111) {
        right_cls = 1;
    } else if (right_ctr == 0) {
        right_cls = 0;
    }

    centre_ctr = ((centre_ctr << 1) & 0b00011111) | (BIT_IS_SET(PINB, 0) || q =='p') ;
    if (centre_ctr == 0b00011111) {
        centre_cls = 1;
    } else if (centre_ctr == 0) {
        centre_cls = 0;
    }
    if (centre_cls != centrePrevious) {
        centrePrevious = centre_cls;
    }

    SW2_ctr = ((SW2_ctr << 1) & 0b00011111) | BIT_IS_SET(PINF, 6);
    if (SW2_ctr == 0b00011111) {
        SW2_cls = 1;
    } else if (SW2_ctr == 0) {
        SW2_cls = 0;
    }

    SW3_ctr = ((SW3_ctr << 1) & 0b00011111) | (BIT_IS_SET(PINF, 5) || q == 't');
    if (SW3_ctr == 0b00011111) {
        SW3_cls = 1;
    } else if (SW3_ctr == 0) {
        SW3_cls = 0;
    }
    if (SW3_cls != SW3Previous) {
        SW3Previous = SW3_cls;
    }
}

void debounceButtons(void) {
    if (SW2_cls != SW2PrevState) {
        SW2PrevState = SW2_cls;
        SW2Press = SW2PrevState;
    }

    if (down_cls == downPrevState) {
        downPress = 0;
    } else if (down_cls != downPrevState) {
        downPress = down_cls;
    }
    downPrevState = down_cls;

    if (SW3_cls == SW3PrevState) {
        SW3Press = 0;
    } else if (SW3_cls != SW3PrevState) {
        SW3Press = SW3_cls;
    }
    SW3PrevState = SW3_cls;

    if (centre_cls != centrePrevState) {
        centrePrevState = centre_cls;
        centrePress = centrePrevState;
    }


   if (up_cls != upPrevState) {
        upPrevState = up_cls;
        upPress = upPrevState;
    }

    if (left_cls != leftPrevState) {
        leftPrevState = left_cls;
        leftPress = leftPrevState;
    }
    if (right_cls != rightPrevState) {
        rightPrevState = right_cls;
        rightPress = rightPrevState;
    }
    
}

void readButtons() {
    if (SW3Press) {
        toggle = !toggle;
    }
}




void teensySetup(void) {
    set_clock_speed(CPU_8MHz);
    enableInput();
    adc_init();
    lcd_init(LCD_DEFAULT_CONTRAST);
    lcd_clear();
    //Initialise timer 0 for debounce
    TCCR0A = 0;
    TCCR0B = 0x04;
    TIMSK0 = 1;

    //Initialise timer 1 in normal mode (code from AMS 9 ex2)
    TCCR1A = 0;
    TCCR1B = 0b00000011;
    TIMSK1 = 1;

    //Initialise timer 3 for zombie led flashing 
    TCCR3A = 0;
    TCCR3B = 0b00000010;
    TIMSK3 = 1;
    /** FROM TOPIC 11 PWM EXAMPLE **/
    TC4H = OVERFLOW_TOP >> 8;
	OCR4C = OVERFLOW_TOP & 0xff;

	TCCR4A = BIT(COM4A1) | BIT(PWM4A);
	SET_BIT(DDRC, 7);

	TCCR4B = BIT(CS42) | BIT(CS41) | BIT(CS40);

	TCCR4D = 0;

    sei();
    /** LED0 and LED 1 */
    SET_BIT(DDRB, 2);
    SET_BIT(DDRB, 3);
    CLEAR_BIT(PORTB, 2);
    CLEAR_BIT(PORTB, 3);

    /** Set up serial communication **/
    usb_init();
}

/** PWM **/
void set_duty_cycle(int duty_cycle) {
	// (a)	Set bits 8 and 9 of Output Compare Register 4A.
	TC4H = duty_cycle >> 8;

	// (b)	Set bits 0..7 of Output Compare Register 4A.
	OCR4A = duty_cycle & 0xff;
}



void freeSprites(void) {
    for(int i = 0; i < block_ctr; i++) {
        free(&blocks[i]);
    }
    for(int j   = 0; j < 5; j++) {
        free(&food[j]);
        free(&zombie[j]);
    }
    free(&hero);
    free(&treasure);
}


void restartGame(void) {
    clear_screen();
    game_over = false;
    end_game = false;
    paused = false;
    toggle = false;
    moveRight = false;
    moveLeft = false;
    movingRight = true; 
    previousBlock = 0;
    currentBlock = 0;
    numberOfZombiesFed = 0;
    previousDrop = 0;
    lives = 10;
    score = 0;
    foodCount = 5;
    zombieCount = 5;
    block_ctr = 0;
    time_at_pause = 0;
    pause_elapsed = current_time();
    previousDrop = pause_elapsed;
    setupGame();
    show_screen();

}

void gameoverMessage(void) {
    usb_serial_send("\n\rGame over");
    livesMessage();
    scoreMessage();
    gameTimeMessage();
    usb_serial_send("\r\nZombies fed: "); usb_serial_send_int((int) numberOfZombiesFed);
}


void gameoverScreen(void) {
    while (wait) {
        int c;
        char minuteS[15];
        char secondS[15];
        char scoreS[15];
        int minutes = floor(endTime/60);
        int seconds = floor(endTime % 60);
        sprintf(minuteS, "%02d", minutes);
        sprintf(secondS, "%02d", seconds);
        sprintf(scoreS, "%d", score);
        if (usb_serial_available()) {
            c = usb_serial_getchar(); 
        } else {
            c = 0;
        }
        clear_screen();

        draw_string(25, 1, "GAME OVER", FG_COLOUR);
        draw_string(10, 11, "Score:", FG_COLOUR);
        draw_string(45, 11, scoreS, FG_COLOUR);
        draw_string(10, 21, "Time:", FG_COLOUR);
        draw_string(45, 21, minuteS, FG_COLOUR);  
        draw_string(55, 21, ":", FG_COLOUR);
        draw_string(59, 21, secondS, FG_COLOUR); 
        draw_string(0, 31, "SW2 to quit", FG_COLOUR);
        draw_string(0, 41, "SW3 to restart", FG_COLOUR);
        show_screen();
        if (BIT_IS_SET(PINF, 5) || c == 'r') {
            restartGame();
            wait = false;
        }
        if (BIT_IS_SET(PINF, 6) || c == 'q') {
            wait = false;
            end_game = true;
            clear_screen();
            draw_string(20, 20, "n9966463", FG_COLOUR);
            show_screen();
        }
    }
}



void pause(void) {
    if (centrePress) {
        pauseMessage();
        paused = true;
        time_at_pause = current_time() - pause_elapsed;
    }
}

void livesDisplay(void) {
    char liveS[15];
    sprintf(liveS, "%d", lives);
    draw_string(10, 1, "Lives:", FG_COLOUR);
    draw_string(45, 1, liveS, FG_COLOUR);
}
void scoreDisplay(void) {
    char scoreS[15];
    sprintf(scoreS, "%d", score);
    draw_string(10, 11, "Score:", FG_COLOUR);
    draw_string(45, 11, scoreS, FG_COLOUR);
}

void timeDisplay(void) {
    char minuteS[15];
    char secondS[15];
    int minutes = floor(time_at_pause/60);
    int seconds = floor(time_at_pause % 60);
    sprintf(minuteS, "%02d", minutes);
    sprintf(secondS, "%02d", seconds);
    draw_string(10, 21, "Time:", FG_COLOUR);
    draw_string(45, 21, minuteS, FG_COLOUR);  
    draw_string(55, 21, ":", FG_COLOUR);
    draw_string(59, 21, secondS, FG_COLOUR);  
}

void foodDisplay(void) {
    char foodS[15];
    sprintf(foodS, "%d", foodCount);
    draw_string(10, 31, "Food:", FG_COLOUR);
    draw_string(45, 31, foodS, FG_COLOUR);
}

void zombieDisplay(void) {
    char zombieS[15];
    sprintf(zombieS, "%d", zombieCount);
    draw_string(10, 41, "Zombies:", FG_COLOUR);
    draw_string(50, 41, zombieS, FG_COLOUR);
}

int c;
void pauseDisplay(void) {
    while (paused){
        centrePress = false; 
        if (usb_serial_available()) {
            c = usb_serial_getchar(); 
        } else {
            c = 0;
        }
        debounceButtons();
        clear_screen();
        livesDisplay();
        scoreDisplay();
        timeDisplay();
        foodDisplay();
        zombieDisplay();
        show_screen();
        if (centrePress || c == 'p') {
            pause_elapsed = current_time() - time_at_pause;
            paused = false;
        }

    }
}



void introScreen(void) {
    bool intro = true;
    int c;
    clear_screen();
    draw_string(18, 11, "Vanessa Li", BG_COLOUR);
    draw_string(20, 20, "n9966463", FG_COLOUR);
    show_screen();
   while (intro) {
       if (usb_serial_available()) {
            c = usb_serial_getchar(); 
        } else {
            c = 0;
        }
        if (BIT_IS_SET(PINF, 6) || c == 's') {
            intro = false;
            pause_elapsed = current_time();
            previousDrop = pause_elapsed;
            break;
        }
   }
}

void process(void) {
    lcd_init(LCD_Contrast);
    if (usb_serial_available()) {
        q = usb_serial_getchar(); 
    } else {
        q = 0;
    }
    set_duty_cycle(backlight);
    debounceButtons();
    readButtons(); 
    pause();
    pauseDisplay();
    heroFunctions();
    blockFunctions();
    treasureFunctions();
    foodFunctions();
    zombieFunctions();
    drawSprites();
    if (lives <= 0) {
        endTime = current_time();
        direct_message();
        if (direct_y > 45) {
            gameoverMessage();
            gameoverScreen();
            wait = true;
            return;
        }
        
    }
}



int main(void) {
    teensySetup();
    introScreen();
    setupGame();
    while (!game_over && !end_game) {
        process();
        _delay_ms(33);
    }
    return 0;
}


