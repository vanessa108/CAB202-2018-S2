#include <avr/io.h>
#include <util/delay.h>
#include <cpu_speed.h>
#include <macros.h>
#include <graphics.h>
#include <lcd.h>
#include "lcd_model.h"

void setup( void ) {
	set_clock_speed(CPU_8MHz);

	//	(a) Enable input from the joystick-up switch and joystick-down switch.
    CLEAR_BIT(DDRD, 1); //up
    CLEAR_BIT(DDRB, 7); //down
	//	(b) Initialise the LCD display using the default contrast setting.
    lcd_init(LCD_DEFAULT_CONTRAST);
	//	(c) Use one of the functions declared in "graphics.h" to display 
	//	your student number, "n9966463", using the foreground colour, 
	//	positioning the left edge of the text at 19 and the nominal top 
	//	of the text at 10.
    clear_screen();
    draw_string(19, 10, "n9966463", FG_COLOUR);
	//	(d) Use one of the functions declared in "graphics.h" to copy the contents 
	//	of the screen buffer to the LCD.
    show_screen();
}

//	(e) Declare a global variable of type int named Lcd_Contrast and 
//	initialise it to the default LCD contrast.
int Lcd_Contrast = LCD_DEFAULT_CONTRAST;

void process(void) {
	//	(f) Test pin corresponding to joystick-down switch. If closed,
	//	decrement Lcd_Contrast by 3. If the result is less than zero,
	//	set Lcd_Contrast to the maximum permitted LCD contrast value.
    
    if (BIT_IS_SET(PINB,7)) {
        Lcd_Contrast = Lcd_Contrast - 3;
        if (Lcd_Contrast < 0) {
            Lcd_Contrast = 127;
        }
    }
    
	//	(g) Test pin corresponding to joystick-up switch. If closed,
	//	increment Lcd_Contrast by 3. If the result is greater
	//	then the maximum permitted LCD contrast value, set Lcd_Contrast 
	//	to zero.
    if (BIT_IS_SET(PIND, 1)) {
        Lcd_Contrast = Lcd_Contrast + 3;
        if (Lcd_Contrast > 127) {
            Lcd_Contrast = 0;
        }
    }
	//	(h) Send a sequence of commands to the LCD to enable extended
	//	instructions, set contrast to the current value of Lcd_Contrast, 
	//	and finally retore the LCD to basic instruction mode.
    lcd_init(Lcd_Contrast);

}


int main(void) {
	setup();

	for ( ;; ) {
		process();
		_delay_ms(100);
	}

	return 0;
}
