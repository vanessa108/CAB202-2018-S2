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

/** Game Constants **/
#define BLOCK_SPEED (0.03)

#define LCD_WIDTH (84)
#define LCD_HEIGHT (48)
#define HERO_HEIGHT (7)
#define HERO_WIDTH (5)
#define BLOCK_HEIGHT (3)
#define BLOCK_WIDTH (10)
#define TREASURE_HEIGHT (7)
#define TREASURE_WIDTH (7)

#define CHANCE_EMPTY (0.35)
#define CHANCE_FORBIDDEN (0.25)

/** Game Booleans **/
bool game_over = false;
bool paused = false;
bool wait = false;
bool toggle = false;
bool end_game = false;

/** Hero Movement **/
bool moveRight = false;
bool moveLeft = false;

/** Game display **/
int lives = 10;
int score = 0;
int foodCount = 5;
int zombieCount = 5;

int block_ctr;
int row[3];
bool movingRight = true;
double block_speed;

bool upPress;
bool downPress;
bool leftPress;
bool rightPress;
bool centrePress;
bool SW2Press;
bool SW3Press;
/** Define Sprites **/
Sprite hero;

Sprite blocks[20];

Sprite treasure;

Sprite food[5];

Sprite zombies[5];

/**Sprite Bitmaps **/ 
uint8_t hero_img [7] = {
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

/* SETUP FUNCTION */

void setupBlocks(void) {
    int num_rows = (LCD_HEIGHT - (TREASURE_HEIGHT + 2))/ (HERO_HEIGHT + 3);
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
                    int row_height = BLOCK_HEIGHT + HERO_HEIGHT + 3;
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
    int leftMost = 0;
    for (int i = 0; i < block_ctr; i++) {
        if (blocks[i].y == row[0]) {
            if (blocks[i].bitmap == safe_img) {
                if (blocks[i].x < blocks[leftMost].x) {
                    leftMost = i; 
                }
            }
        }
    }
    sprite_init(&hero, blocks[leftMost].x + 4, blocks[leftMost].y - 7, HERO_WIDTH, HERO_HEIGHT, hero_img);
}

void setupTreasure(void) {
    sprite_init(&treasure, 0, LCD_HEIGHT-7, 7, 7, treasure_img);
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
    show_screen();
}

void setupGame(void) { 
    setupBlocks();
    setupHero();
    setupTreasure();
    setupFood();
}

/** Game Mechanics **/

void heroRespawn(void) {
    free(&hero);
    moveRight = false;
    moveLeft = false;
    setupHero();
}

spritePos_t spritePos(Sprite s) {
    spritePos_t p;
    p.top = s.y;
    p.bottom = s.y + s.height;
    p.left = s.x;
    p.right = s.x + s.width;
    return p;
}

int previousBlock;
int currentBlock;
bool isHeroStanding(void) {
    bool isStand = false;
    for (int i = 0; i < block_ctr; i++) {
       spritePos_t blockPos = spritePos(blocks[i]);
       spritePos_t heroPos = spritePos(hero); 
       if (heroPos.bottom == blockPos.top  && heroPos.right <= blockPos.right + 5 && heroPos.left >= blockPos.left - 5) {
            if (blocks[i].bitmap == safe_img) {
                isStand = true;
                currentBlock = i;
                hero.y = blockPos.top - 7;
            } else {
                lives -= 1;
                heroRespawn();
            }
       }
    }
    return isStand;
}

void heroOffscreen(void) {
    spritePos_t heroPos = spritePos(hero);
    if (heroPos.bottom > LCD_HEIGHT || heroPos.top < 0 ||
    heroPos.left < 0 || heroPos.right > LCD_WIDTH) {
        lives -= 1;
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
    //bool heroStand = isHeroStanding();
    //if (heroStand) {
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
    //}
}

void heroMovement(void) {
    bool heroStand = isHeroStanding();
    if (upPress) {
        if (heroStand) {
        hero.dy = -2.2;
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
            hero.dx = block_speed + 0.4;
        } else {
            hero.dx = block_speed + 0.5;
        }
    } else if (moveLeft) {
        if (hero.y + HERO_HEIGHT == row[1]) {
            hero.dx = -(block_speed + 0.4);
        }
        hero.dx = -(block_speed + 0.5);
    }
    hero.x += hero.dx;
}

void heroGravity(void) {
    bool heroStand = isHeroStanding();
    if (!heroStand) {
        hero.dy += 0.2;
        if (hero.dy > 1) {
            hero.dy = 1;
        }
        moveRight = false;
        moveLeft = false;
    }
    if (heroStand) {
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
    }
}



void heroFunctions(void) {
    heroGravity();;
    heroControls();
    heroMovement();
    heroOffscreen();
    heroTreasure();
    scoreOnBlock();
}

/** Zombie and Food **/
void dropFood(void) {
    bool heroStand = isHeroStanding();
    int fc = 5 - foodCount;
    if (downPress && foodCount > 0 && heroStand) {
        food[fc].x = hero.x + 2;
        food[fc].y = hero.y + 5;
        food[fc].is_visible = 1;
        foodCount -= 1;
    }
}

void foodOnBlock(void) {
    //bool foodBlock = false;
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


void zombieFunctions(void) {
    dropFood();
    foodOnBlock();
}



int main(void);

void restartGame(void) {
    game_over = false;
    paused = false;
    toggle = false;
    moveRight = false;
    moveLeft = false;
    lives = 10;
    score = 0;
    foodCount = 5;
    zombieCount = 5;
    main();
}



void gameoverScreen(void) {
    while (wait) {
        clear_screen();
        draw_string(10, 10, "GAME OVER", FG_COLOUR);
        show_screen();
        if (BIT_IS_SET(PINF, 5)) {
            restartGame();
            wait = false;
        }
        if (BIT_IS_SET(PINF, 6)) {
            wait = false;
            end_game = true;
            clear_screen();
            draw_string(20, 20, "n9966463", FG_COLOUR);
            show_screen();
        }
    }
}


void enableInput(void) {
    SET_INPUT(DDRD, 1); //up
    SET_INPUT(DDRB, 1); //left
    SET_INPUT(DDRD, 0); //right
    SET_INPUT(DDRB, 7); //down 
    SET_INPUT(DDRB, 0); //centre
    SET_INPUT(DDRF, 5); //right button (SW3)
    SET_INPUT(DDRF, 6);  //left button (SW2)
}

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

bool upPrevious = false; 
bool downPrevious = false;
bool leftPrevious = false;
bool rightPrevious = false;
bool centrePrevious = false;
bool SW2Previous = false;
bool SW3Previous = false;


ISR(TIMER0_OVF_vect) {
    up_ctr = ((up_ctr << 1) & 0b00011111) | BIT_IS_SET(PIND, 1);
    if (up_ctr == 0b00011111) {
        up_cls = 1;
    } else if (up_ctr == 0) {
        up_cls = 0;
    }

    down_ctr = ((down_ctr << 1) & 0b00011111) | BIT_IS_SET(PINB, 7);
    if (down_ctr == 0b00011111) {
        down_cls = 1;
    } else if (down_ctr == 0) {
        down_cls = 0;
    }
    if (down_cls != downPrevious) {
        downPrevious = down_cls;
    }

    left_ctr = ((left_ctr << 1) & 0b00011111) | BIT_IS_SET(PINB, 1);
    if (left_ctr == 0b00011111) {
        left_cls = 1;
    } else if (left_ctr == 0) {
        left_cls = 0;
    }

    right_ctr = ((right_ctr << 1) & 0b00011111) | BIT_IS_SET(PIND, 0);
    if (right_ctr == 0b00011111) {
        right_cls = 1;
    } else if (right_ctr == 0) {
        right_cls = 0;
    }

    centre_ctr = ((centre_ctr << 1) & 0b00011111) | BIT_IS_SET(PINB, 0);
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

    SW3_ctr = ((SW3_ctr << 1) & 0b00011111) | BIT_IS_SET(PINF, 5);
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
    downPress = down_cls == downPrevState ? 0 : down_cls;
    downPrevState = down_cls;

    SW3Press = SW3_cls == SW3PrevState ? 0 : SW3_cls;
    SW3PrevState = SW3_cls;

    // centrePress = centre_cls == centrePrevState ? 0 : centre_cls;
    // centrePrevState = centre_cls;
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
    if (SW2_cls != SW2PrevState) {
        SW2PrevState = SW2_cls;
        SW2Press = SW2PrevState;
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
   sei();
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
    char timeS[15];
    sprintf(timeS, "%d", score);
    draw_string(10, 21, "Time:", FG_COLOUR);
    draw_string(45, 21, timeS, FG_COLOUR);  
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

void pauseDisplay(void) {
    while (paused){
        centrePress = false;
        debounceButtons();
        clear_screen();
        livesDisplay();
        scoreDisplay();
        timeDisplay();
        foodDisplay();
        zombieDisplay();
        show_screen();
        if (centrePress) {
            paused = false;
        }

    }
}

void pause(void) {
    if (centrePress) {
        paused = true;
    }
}

void introScreen(void) {
   clear_screen();
   draw_string(18, 11, "Vanessa Li", BG_COLOUR);
   draw_string(20, 20, "n9966463", FG_COLOUR);
   show_screen();
   while (!BIT_IS_SET(PINF, 6)) {} 
}

void process(void) {
    debounceButtons();
    readButtons(); 
    pause();
    pauseDisplay();
    heroFunctions();
    blockFunctions();
    treasureFunctions();
    zombieFunctions();
    drawSprites();
    if (lives <= 0) {
        gameoverScreen();
        wait = true;
        return;
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


