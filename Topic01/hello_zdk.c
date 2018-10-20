#include <stdlib.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

// Configuration
#define DELAY (10) /* Millisecond delay between game updates */

// Game state.
bool game_over = false;

// Implementation
void setup( void ) {
	draw_string( 0, 0, "Hello CAB202." );
}

void process( void ) {
	// Nothing happens in this game.
}

void cleanup( void ) {
	// Nothing to clean up in this game.
}

// Program entry point.
int main( void ) {
	setup_screen();
	setup();
	show_screen();

	while ( ! game_over ) {
		process();
		show_screen();
		timer_pause( DELAY );
	}

	return 0;
}
