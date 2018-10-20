#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <math.h>
#include <string.h>
//#include "draw_formatted_lines.h"

/** CONSTANTS PART A **/
#define DELAY (10)
#define END (200)

#define CAR_HEIGHT (4)
#define CAR_WIDTH (6)

#define FUELBAY_HEIGHT (6)
#define FUELBAY_WIDTH (8)
#define FUELDEPOT_HEIGHT (4)
#define FUELDEPOT_WIDTH (5)

#define ZOMBIE_WIDTH (5)
#define ZOMBIE_HEIGHT (3)
#define TREE_WIDTH (12)
#define TREE_HEIGHT (6)
#define ROADBLOCK_WIDTH (10)
#define ROADBLOCK_HEIGHT (3)

#define MAX_OBSTACLES (2)
#define MAX_TREES (3)

#define DASH_HEIGHT (7)
#define DASH_WIDTH (21)
#define MOUNTAIN_WIDTH (32)
#define MOUNTAIN_HEIGHT (6)

/** GLOBAL VARIABLES PART A**/
bool game_over = false;
bool update_screen = true;
bool fuelbay_exists = false;
bool fuelling = false;
bool finishline_visible = false;

timer_id fuel_timer;
double dist_time = 0;
double start_time;
double current_time = 0;
double fuel_time = 0;
double speed = 0;
double fuel = 100;
double distance = 0;
int health = 100;
int road_left, road_right;

sprite_id race_car;
sprite_id fuelbay;
sprite_id fueldepot;
sprite_id zombie;
sprite_id tree;
sprite_id roadblock;
sprite_id mountain;
sprite_id zombies[MAX_OBSTACLES];
sprite_id trees[MAX_TREES];
sprite_id roadblocks[MAX_OBSTACLES];

char * welcome_msg =
/**/    "      Race to Zombie Mountain     "
/**/    "     By: Riva Mendoza N9941215    "
/**/    "                                  "
/**/    "         ---Commands---           "
/**/    "         accelerate:  W           "
/**/    "         decelerate:  s           "
/**/    "         left:        A           "
/**/    "         right:       D           "
/**/    "                                  "
/**/    "    Press any key to continue...  ";

char * error_msg =
/**/	"Error: Terminal window too small."
/**/	"     Exit game and try again.    ";

char * racecar_img =
/**/"{}--{}"
/**/"  /\\  "
/**/"  \\/  "
/**/"{}--{}";

char * zombie_img =
/**/"(-.-)"
/**/" -|- "
/**/" / \\  ";

char * tree_img=
/**/"     &&&     "
/**/" &\\/&|&&&&  "
/**/"&&&\\&|&/&&& "
/**/" &&&\\|/&&&  "
/**/"    | |      "
/**/"   | |       ";

char * roadblock_img =
/**/ "  _    _  "
/**/ "-| |--| |-"
/**/ "-| |--| |-";

char * fueldepot_img =
/**/ " ___ "
/**/ "/   \\"
/**/ "| $ |"
/**/ "|___|";

char * fuelbay_img =
/**/"$$$$$$$$"
/**/"$      $"
/**/"$      $"
/**/"$      $"
/**/"$      $"
/**/"$$$$$$$$";

char * health_msg =
/**/	"   You died!   "
/**/  "Play again? Y/N";

char * fuel_msg =
/**/	"You ran out of fuel!"
/**/  "  Play again? Y/N   ";

char * crash_msg =
/**/	"You crashed into the fuel depot!"
/**/  "        Play again? Y/N         ";

char * win_msg =
/**/	"Congratulations! You made it to Zombie Mountain!"
/**/  "               Play again? Y/N                  ";

char * mountain_img =
/**/ "         /\\     __   /\\         "
/**/ "     _/\\/  \\_  /  \\_/  \\__      "
/**/ "    /   \\    \\/        /  \\     "
/**/ "   /     \\_  /       _/    \\    "
/**/ "  /        \\/       /       \\   "
/**/ " /         /       /         \\  ";

/** FUNCTIONS PART A**/
//setup sprites
void setup_car(void);
void setup_roadblocks(void);
void setup_zombies(void);
void setup_trees(void);
void setup_fueldepot(void);
void setup_mountain(void);

//draw objects
void draw_game(void);
void welcome_screen(void);
void draw_border(void);
void draw_dashboard(void);
void draw_road(void);
void draw_finishline(void);
void draw_all_sprites(void);

//update sprites
void move_racecar(void);
void update_car(int key);
void update_sprites(sprite_id sprites[], int num_sprites);
void update_sprite(sprite_id sprite);
void process_fuel(sprite_id fuelbay, sprite_id fueldepot);
void process_finishline(sprite_id mountain);

//calculation functions
void refuel(void);
void fuel_calc(void);
void distance_calc(void);
void health_calc(void);

//game over functions
void game_over_screen(char * msg);
void cleanup(void);
void reset_game(void);

//boolean functions
bool car_in_fuelbay(sprite_id car, sprite_id fuelbay);
bool sprites_collided(sprite_id sprite1, sprite_id sprite2);
bool sprite_on_roadline(sprite_id sprite);
bool offroad(void);

//sprite_id functions
sprite_id sprite_collided_with_any(sprite_id sprite, sprite_id sprites[], int num_sprites);
sprite_id setup_a_zombie(void);
sprite_id setup_a_tree(void);
sprite_id setup_a_roadblock(void);

/** CONSTANTS PART B **/
#define MAX_ITEMS (40)
#define ITEM_HEIGHT (1)
#define ITEM_WIDTH (5)
#define MAX_ROUNDS (1000)

/** GLOBAL VARIABLES PART B**/
sprite_id items[MAX_ITEMS];
int item_images[MAX_ITEMS];
int num_items;
int score = 0;
int game_round = 1;

/** FUNCTIONS PART B **/
void setup_items(void);
void update_item(sprite_id item, int index);
void update_items(sprite_id items[], int num_items);
void item_got(sprite_id item, int index);
sprite_id setup_an_item(int index, int num_items);

/** PART B CODE **/
void setup_items(void)
{
  int now = get_current_time();
  srand(now);

  if( rand() % 5 == 0 )
  {
    num_items = 40;
  }
  else
  {
    num_items = 5;
  }

  for(int i = 0; i < num_items; i++)
  {
    bool ok = false;
    while(!ok)
    {
      items[i] = setup_an_item(i, num_items);
      ok = true;

      for(int j = 0; j < i; j++)
      {
        if(sprites_collided(items[i], items[j]))
        {
          ok = false;
          break;
        }
      }
    }
  }
}

sprite_id setup_an_item(int index, int num_items)
{
  char * item_img []=
  {/**/ "( $ )",
   /**/ "( + )",
   /**/ "(>$<)",
   /**/ "(>+<)"};
  int item_ID = rand() % 2;
  if(num_items == 40)
  {
    item_ID += 2;
  }
  item_images[index] = item_ID;

  int x_range = screen_width() - ITEM_WIDTH - 2;
  int item_x = rand() % x_range + 1;
  int item_y = rand() % screen_height()-1;

  sprite_id item = sprite_create(item_x, item_y, ITEM_WIDTH, ITEM_HEIGHT, item_img[item_ID]);

  //make sure it doesn't collide with car or roadblocks
  if (sprites_collided(item, race_car) || sprite_collided_with_any(item, roadblocks, MAX_OBSTACLES) ||
      sprite_on_roadline(item) || sprite_collided_with_any(item, trees, MAX_TREES) || sprite_collided_with_any(item, zombies, MAX_OBSTACLES))
  {
    return setup_an_item(index,num_items);
  }
  item = sprite_create(item_x, -item_y, ITEM_WIDTH, ITEM_HEIGHT, item_img[item_ID]);
  return item;
}

void update_items(sprite_id items[], int num_items)
{
  int count = 0;
  for ( int i = 0; i < num_items; i++ )
  {
      sprite_id current_sprite = items[i];
      update_item(current_sprite, i);
      if(round(sprite_y(current_sprite)) > screen_height())
      {
        sprite_hide(current_sprite);
        count++;
      }
  }

  if(count == num_items && distance < END)
  {
    return setup_items();
  }
}

void update_item(sprite_id item, int index)
{
  sprite_step(item);

  double dy = speed/10;
  sprite_turn_to(item, 0, dy);

  if(sprites_collided(race_car, item))
  {
    item_got(item, index);
  }
}

void item_got(sprite_id item, int index)
{
  if(sprite_visible(item))
  {
    if(item_images[index] == 0)
    {
      score++;
    }
    else if(item_images[index] == 2)
    {
      score+=5;
    }
    else if(health < 100)
    {
      if(item_images[index] == 1)
      {
        health++;
      }
      else if(item_images[index] == 3)
      {
        if(health > 95)
        {
          health = 100;
        }
        else
        {
          health+=5;
        }
      }
    }
  }
  sprite_hide(item);
}

/** PART A CODE **/
/** SETUP **/
void setup(void)
{
  setup_car();
  setup_roadblocks();
  setup_zombies();
  setup_trees();
  setup_items();
  setup_fueldepot();
  setup_mountain();
  setup_items();
  fuel_timer = create_timer(3000);
  start_time = get_current_time();
  draw_game();
}

void setup_car(void)
{
  int car_x =(screen_width() - CAR_WIDTH) / 2;
  int car_y =(screen_height()-1) - (screen_height() * 25/100);
  race_car = sprite_create(car_x, car_y, CAR_WIDTH, CAR_HEIGHT, racecar_img);
}

void setup_roadblocks(void)
{
  int now = get_current_time();
  srand(now);

  for(int i = 0; i < MAX_OBSTACLES; i++)
  {
    bool ok = false;
    while(!ok)
    {
      roadblocks[i] = setup_a_roadblock();
      ok = true;

      for(int j = 0; j < i; j++)
      {
        if(sprites_collided(roadblocks[i], roadblocks[j]))
        {
          ok = false;
          break;
        }
      }
    }
  }
}

sprite_id setup_a_roadblock(void)
{
  road_left = screen_width()/2 - (screen_width()/5);
  road_right = screen_width()/2 + (screen_width()/5);

  int h = screen_height();
	int roadblock_x = (road_left+1) + rand() % ((road_right-ROADBLOCK_WIDTH-1) - (road_left+1));
	int roadblock_y = rand() % (h - ROADBLOCK_HEIGHT-CAR_HEIGHT - 5);

  sprite_id roadblock = sprite_create(roadblock_x, roadblock_y, ROADBLOCK_WIDTH, ROADBLOCK_HEIGHT, roadblock_img);
  if(sprites_collided(race_car, roadblock))
  {
    setup_a_roadblock();
  }
  return roadblock;
}

void setup_zombies(void)
{
  int now = get_current_time();
  srand(now);

  for(int i = 0; i < MAX_OBSTACLES; i++)
  {
    bool ok = false;
    while(!ok)
    {
      zombies[i] = setup_a_zombie();
      ok = true;

      for(int j = 0; j < i; j++)
      {
        if(sprites_collided(zombies[i], zombies[j]))
        {
          ok = false;
          break;
        }
      }
    }
  }
}

sprite_id setup_a_zombie(void)
{
  int x_range = screen_width() - ZOMBIE_WIDTH - 2;
  int y_range = screen_height() - ZOMBIE_HEIGHT - 2;
  int zombie_x = rand() % x_range + 1;
  int zombie_y = rand() % (y_range + 1 - DASH_HEIGHT + 3) + DASH_HEIGHT + 3;

  zombie = sprite_create(zombie_x, zombie_y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, zombie_img);

  //make sure it doesn't collide with car or roadblocks
  if (sprites_collided(zombie, race_car) || sprite_collided_with_any(zombie, roadblocks, MAX_OBSTACLES) || sprite_on_roadline(zombie))
  {
    return setup_a_zombie();
  }
  return zombie;
}

void setup_trees(void)
{
  int now = get_current_time();
  srand(now);

  for(int i = 0; i < MAX_TREES; i++)
  {
    bool ok = false;
    while(!ok)
    {
      trees[i] = setup_a_tree();
      ok = true;

      for(int j = 0; j < i; j++)
      {
        if(sprites_collided(trees[i], trees[j]))
        {
          ok = false;
          break;
        }
      }
    }
  }
}

sprite_id setup_a_tree(void)
{
  int x_range = screen_width() - TREE_WIDTH - 2;
  int y_range = screen_height() - TREE_HEIGHT - 2;
  int tree_x[] = {rand() % (road_left - TREE_WIDTH) + 1,
                  rand() % (x_range + 1 - road_right) + road_right};
  int tree_y[] = {9 + rand() % y_range, 3 + rand() % y_range};
  int side = rand() % 2;

  tree = sprite_create(tree_x[side], tree_y[side], TREE_WIDTH, TREE_HEIGHT, tree_img);

  // checks zombie collision
  if (sprite_collided_with_any(tree, zombies, MAX_OBSTACLES))
  {
    return setup_a_tree();
  }

  return tree;
}

void setup_fueldepot(void)
{
  int fuelbay_x[] ={road_left+1, road_right-FUELBAY_WIDTH};
  int fueldepot_x[] ={road_left-FUELDEPOT_WIDTH, road_right+1};
  int fueldepot_y = rand() % 100;
  int fuel_side = rand() % 2;

  fueldepot = sprite_create(fueldepot_x[fuel_side], -fueldepot_y, FUELDEPOT_WIDTH, FUELDEPOT_HEIGHT, fueldepot_img);
  fuelbay = sprite_create(fuelbay_x[fuel_side], -fueldepot_y, FUELBAY_WIDTH, FUELBAY_HEIGHT, fuelbay_img);

  if (sprite_collided_with_any(fueldepot, items, num_items) ||sprite_collided_with_any(fueldepot, zombies, MAX_OBSTACLES) || sprite_collided_with_any(fueldepot, roadblocks, MAX_OBSTACLES) || sprite_collided_with_any(fueldepot, trees, MAX_TREES)
       ||sprite_collided_with_any(fuelbay, items, num_items) ||sprite_collided_with_any(fuelbay, zombies, MAX_OBSTACLES) || sprite_collided_with_any(fuelbay, roadblocks, MAX_OBSTACLES) || sprite_collided_with_any(fuelbay, trees, MAX_TREES))
  {
    setup_fueldepot();
  }
}

void setup_mountain(void)
{
  int fin_x =(screen_width() - MOUNTAIN_WIDTH)/2;
  int fin_y = -100;
  mountain = sprite_create(fin_x, fin_y, MOUNTAIN_WIDTH, MOUNTAIN_HEIGHT, mountain_img);
}

/** PROCESS ONE STEP **/
void process(void)
{
  current_time = get_current_time() - start_time;

  int key = get_char();

  update_car(key);
  if(!(sprite_collided_with_any(race_car, roadblocks, MAX_OBSTACLES) || sprite_collided_with_any(race_car, zombies, MAX_OBSTACLES) || sprite_collided_with_any(race_car, trees, MAX_TREES)))
  {
    update_sprites(roadblocks, MAX_OBSTACLES);
    update_sprites(zombies, MAX_OBSTACLES);
    update_sprites(trees, MAX_TREES);
    update_items(items, num_items);

    if(fuelbay_exists)
    {
      process_fuel(fuelbay, fueldepot);
    }

    if(finishline_visible)
    {
      process_finishline(mountain);
    }

    fuel_calc();
    distance_calc();
  }
  draw_game();
}

void process_fuel(sprite_id fuelbay, sprite_id fueldepot)
{
  update_sprite(fuelbay);
  update_sprite(fueldepot);

  if(sprites_collided(race_car, fueldepot))
  {
    health = 0;
    game_over_screen(crash_msg);
  }

  if(sprite_y(fueldepot)>screen_height())
  {
    fuelbay_exists = false;
  }
  else
  {
    if(!fuelling && car_in_fuelbay(race_car, fuelbay))
    {
      timer_reset(fuel_timer);
      fuelling = true;
    }
    if(fuelling && timer_expired(fuel_timer) && car_in_fuelbay(race_car, fuelbay))
    {
      refuel();
      fuelling = false;
    }
  }
}

void process_finishline(sprite_id mountain)
{
  update_sprite(mountain);
  if(round(sprite_y(race_car)) <= round(sprite_y(mountain))+7 || sprites_collided(race_car, mountain))
  {
    game_over_screen(win_msg);
  }
}

/** DRAW GAME **/
void draw_game(void)
{
  clear_screen();
  draw_road();
  draw_all_sprites();
  sprite_draw(race_car);
  draw_dashboard();
  draw_border();
}

void draw_all_sprites(void)
{
  int chance = rand() % (int)fuel;

  for ( int i = 0; i < MAX_OBSTACLES; i++ )
  {
    sprite_draw(roadblocks[i]);
    sprite_draw(zombies[i]);
  }

  for ( int i = 0; i < MAX_TREES; i++ )
  {
   sprite_draw(trees[i]);
  }

  for (int i = 0; i < num_items; i++)
  {
    sprite_draw(items[i]);
  }

  if(fuelbay_exists)
  {
    sprite_draw(fuelbay);
    sprite_draw(fueldepot);
  }
  else if(chance == 1 && distance < END)
  {
    fuelbay_exists = true;
    setup_fueldepot();
    sprite_draw(fuelbay);
    sprite_draw(fueldepot);
  }

  if(finishline_visible)
  {
    sprite_draw(mountain);
    draw_finishline();
  }
}

void welcome_screen(void)
{
  if(screen_width() >= 80 && screen_height() >= 24)
  {
    int msg_height = 10;
    int msg_width = strlen(welcome_msg) / msg_height;
    sprite_id msg_box = sprite_create((screen_width() - msg_width) / 2,
        (screen_height() - msg_height) / 2,
        msg_width, msg_height, welcome_msg);
    sprite_draw(msg_box);
    show_screen();
    while ( get_char() >= 0 ) { }
    wait_char();
  }
  else
  {
    int msg_height = 2;
	  int msg_width = strlen(error_msg) / msg_height;
	  sprite_id msg_box = sprite_create((screen_width() - msg_width) / 2,
        (screen_height() - msg_height) / 2,
	      msg_width, msg_height, error_msg);
	  sprite_draw(msg_box);
	  show_screen();
    while ( get_char() >= 0 ) { /* loop until no chars buffered */ }
    wait_char();
    cleanup();
  }
}

void draw_border(void)
{
  int left = 0;
  int right = screen_width() - 1;
  int top = 0;
  int bottom = screen_height()-1;

  draw_line(left, top, left, bottom, '.');
  draw_line(right, top, right, bottom, '.');
  draw_line(left, top, right, top, '.');
  draw_line(left, bottom, right, bottom, '.');
}

void draw_dashboard(void)
{
  int left = 2;
  int right = DASH_WIDTH+3;
  int top = 2;
  int bottom = DASH_HEIGHT+3;

  draw_formatted(left+1, top+1, "\t\t\t\t\t\tRound%2d\t\t\t\t\t\t\t\t", game_round);
  draw_formatted(left+1, top+2, "Time:\t\t\t\t\t%5.2fs\t\t\t\t\t\t\t\t", current_time);
  draw_formatted(left+1, top+3, "Health:%5d/100\t\t\t\t\t", health);
  draw_formatted(left+1, top+4, "Speed:\t\t\t\t%5.2f m/s\t\t", speed);
  draw_formatted(left+1, top+5, "Fuel:\t\t\t\t\t%5.2f L\t\t\t\t", fuel);
  draw_formatted(left+1, top+6, "Distance:\t%5.2f m\t\t\t\t", distance);
  draw_formatted(left+1, top+7, "Score:\t%5d coins\t\t\t\t", score);

  draw_line(left, top, left, bottom, '*');
  draw_line(right, top, right, bottom, '*');
  draw_line(left, top, right, top, '*');
  draw_line(left, bottom, right, bottom, '*');
}

void draw_road(void)
{
	int top = 1;
	int bottom = screen_height()-1;

	draw_line(road_left, top, road_left, bottom, '|');
	draw_line(road_right, top, road_right, bottom, '|');
}

void draw_finishline(void)
{
  //delete road behind
  draw_line(road_left,sprite_y(mountain)+MOUNTAIN_HEIGHT+2, road_left, 1, ' ');
  draw_line(road_right,sprite_y(mountain)+MOUNTAIN_HEIGHT+2, road_right, 1, ' ');

  //draw "finish"
  draw_formatted((screen_width()/2)-5, sprite_y(mountain)+MOUNTAIN_HEIGHT+2, "F I N I S H");

  //draw finish lines
  draw_line(1, sprite_y(mountain)+MOUNTAIN_HEIGHT, screen_width()-1, sprite_y(mountain)+MOUNTAIN_HEIGHT, '_');
  draw_line(1, sprite_y(mountain)+MOUNTAIN_HEIGHT+3, screen_width()-1, sprite_y(mountain)+MOUNTAIN_HEIGHT+3, '_');
}

/** UPDATES **/
void update_sprites(sprite_id sprites[], int num_sprites)
{
  for ( int i = 0; i < num_sprites; i++ )
  {
      sprite_id current_sprite = sprites[i];
      update_sprite(current_sprite);
  }
}

void update_sprite(sprite_id sprite)
{
  sprite_step(sprite);

  int y = round(sprite_y(sprite));

  double dy = speed/10;
  sprite_turn_to(sprite, 0, dy);

  // Test to see if the sprite hit the top or bottom border.
  if(distance < END && y == screen_height() && !sprites_equal(sprite, fuelbay) && !sprites_equal(sprite, fueldepot))
  {
    sprite_move (sprite, 0, 0-sprite_height(sprite) - screen_height() - sprite_height(sprite));
  }
}

void update_car(int key)
{
  int maxspeed[2] = {10,3};
  int car_left = round(sprite_x(race_car));

  while(sprite_x(race_car) < 1 || sprite_x(race_car) + CAR_WIDTH > screen_width()-1 || sprite_collided_with_any(race_car, trees, MAX_TREES) || sprite_collided_with_any(race_car, zombies, MAX_OBSTACLES) || sprite_collided_with_any(race_car, roadblocks, MAX_OBSTACLES))
  {
    move_racecar();
  }

  if ((key == 'a' || key == 'A')
       && car_left > 1
       && speed > 0)
	{
    sprite_move( race_car, -1, 0);

		if (offroad() && speed > maxspeed[1])
		{
			speed = maxspeed[1];
		}
	}

  else if ((key == 'd' || key == 'D')
           && car_left < (screen_width() - CAR_WIDTH-1)
           && speed > 0)
	{
    sprite_move( race_car, +1, 0 );

    if (offroad() && speed > maxspeed[1])
    {
      speed = maxspeed[1];
    }
	}

	else if ((key == 'w' || key == 'W') )
	{
    if(offroad() && speed < maxspeed[1])
    {
		  speed++;
    }
	  else if(!offroad() && speed < maxspeed[0])
    {
			speed++;
    }
	}

	else if ((key == 's' || key == 'S') && speed > 0)
	{
		speed--;
	}
}

void move_racecar(void)
{
  int new_carx = sprite_x(race_car);
  int new_cary = (screen_height()-1) - (screen_height() * 25/100);

  int side = rand() % 2;

  if(side == 0)
  {
    new_carx++;
  }
  else
  {
    new_carx--;
  }

  health_calc();

  speed = 0;
  refuel();

  sprite_move_to(race_car, new_carx, new_cary);
}

/** COLLISION **/
bool offroad(void)
{
  int car_left = round(sprite_x(race_car));
  int car_right = round(sprite_x(race_car)) + CAR_WIDTH;

  if(car_left <= road_left || car_right > road_right)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool sprite_on_roadline(sprite_id sprite)
{
  int sprite_left = round(sprite_x(sprite));
  int sprite_right = round(sprite_x(sprite)) + sprite_width(sprite);
  for(int j = sprite_left; j < sprite_right; j++)
  {
    if(j == road_left || j == road_right)
    {
      return true;
    }
  }
  return false;
}

bool sprites_collided(sprite_id sprite_1, sprite_id sprite_2)
{
  int sprite_1_top = round(sprite_y(sprite_1)),
      sprite_1_bottom = sprite_1_top + sprite_height(sprite_1),
      sprite_1_left = round(sprite_x(sprite_1)),
      sprite_1_right = sprite_1_left + sprite_width(sprite_1);

  int sprite_2_top = round(sprite_y(sprite_2)),
      sprite_2_bottom = sprite_2_top + sprite_height(sprite_2),
      sprite_2_left = round(sprite_x(sprite_2)),
      sprite_2_right = sprite_2_left + sprite_width(sprite_2);

  if(sprite_1_bottom < sprite_2_top || sprite_1_top > sprite_2_bottom || sprite_1_right < sprite_2_left || sprite_1_left > sprite_2_right)
  {
    return false;
  }
  return true;
}

sprite_id sprite_collided_with_any(sprite_id sprite, sprite_id sprites[], int num_sprites)
{
  for ( int i = 0; i < num_sprites; i++ )
  {
      sprite_id current_sprite = sprites[i];

      if ( sprite != current_sprite && sprites_collided(sprite, current_sprite) )
      {
          return current_sprite;
      }
  }

  return NULL;
}

bool car_in_fuelbay(sprite_id car, sprite_id fuelbay)
{
  int car_top = round(sprite_y(car));
  int car_left = round(sprite_x(car));
  int fuelbay_top = round(sprite_y(fuelbay));
  int fuelbay_left = round(sprite_x(fuelbay));

  int top_collision = car_top - fuelbay_top;
  int left_collision = car_left - fuelbay_left;

  if(top_collision == 1 && left_collision == 1)
  {
    return true;
  }

  return false;
}

/* CALCULATIONS */
void refuel(void)
{
  fuel = 100;
}

void fuel_calc(void)
{
  fuel -= speed/100;
  if(fuel < 1)
  {
    game_over_screen(fuel_msg);
  }
}

void distance_calc(void)
{
  if(distance >= END)
  {
    finishline_visible = true;
  }
  double seconds = get_current_time() - dist_time;
  distance += speed * seconds;
  dist_time = get_current_time();
}

void health_calc(void)
{
  if (sprite_collided_with_any(race_car, zombies, MAX_OBSTACLES) )
  {
    health -= 2 * speed;
  }

  else if ((sprite_collided_with_any(race_car, trees, MAX_TREES) || sprite_collided_with_any(race_car, roadblocks, MAX_OBSTACLES))  )
  {
    health -= speed;
  }

  if(health <= 0)
  {
    health = 0;
    game_over_screen(health_msg);
  }
}

//cleanup game
void cleanup(void)
{
  exit(0);
}

void game_over_screen(char * msg)
{
  clear_screen();
  int msg_height = 2;
  int msg_width = strlen(msg) / msg_height;

  int x = (screen_width() - msg_width) / 2;
  int y = (screen_height() - msg_height) / 2;

  sprite_id msg_box = sprite_create(x, y, msg_width, msg_height, msg);
  sprite_draw(msg_box);

  draw_formatted((screen_width() - 48) / 2, y+3,  "-----------------------STATS--------------------");
  draw_formatted((screen_width() - 48) / 2, y+4,  "Elapsed Time (seconds):                  %5.2f  ", current_time);
  draw_formatted((screen_width() - 48) / 2, y+5,  "Overall Distance (metres):               %5.2f  ", distance);
  draw_formatted((screen_width() - 48) / 2, y+6,  "Coins Collected:                         %5d   ", score);
  draw_formatted((screen_width() - 48) / 2, y+7,  "Health (/100):                           %5d   ", health);

  show_screen();
  int key = wait_char();

  while (!((key == 'Y' || key == 'y') && (key == 'n' || key == 'N')))
  {
    if(key == 'Y' || key == 'y')
    {
      reset_game();
    }
    else if (key == 'N' || key == 'n')
    {
      game_over = true;
      cleanup();
    }
    else
    {
      key = wait_char();
    }
  }
}

/** MAIN **/
int main(void)
{
  setup_screen();
  welcome_screen();
  setup();
  show_screen();

  while (!game_over)
  {
    process();

    if (update_screen)
    {
      show_screen();
    }
    timer_pause(DELAY);
  }
}

void reset_game(void)
{
  clear_screen();
  current_time = 0;
  health = 100;
  distance = 0;
  fuel = 100;
  speed = 0;
  score = 0;
  fuelbay_exists = false;
  fuelling = false;
  finishline_visible = false;
  game_round++;

  main();
}
