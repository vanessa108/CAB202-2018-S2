# Topic 10 Makefile (with USB Serial and UART)
# Lawrence Buckingham, October  2017.
# Queensland University of Technology.

# Replace these targets with your target (hex file) name, including the .hex part.

TARGETS = \
	usb_serial_led.hex \
	usb_serial_hello.hex \
	usb_serial_echo.hex	\
	usb_sprite.hex 	\
	usb_game_controller.hex \
	usb_zdk.exe

# Set the name of the folder containing libcab202_teensy.a
CAB202_TEENSY_FOLDER= c:/CAB202/cab202_teensy 

# Set the name of the folder containing usb_serial.o
USB_SERIAL_FOLDER = c:/CAB202/usb_serial
USB_SERIAL_OBJ = c:/CAB202/usb_serial/usb_serial.o

# Set the name of the folder containing uart.o
UART_FOLDER = c:/CAB202/uart
UART_OBJ = c:/CAB202/uart/uart.o

# Set the name of the folder containing the ZDK library libzdk.a
ZDK_FOLDER = c:/CAB202/ZDK

# ---------------------------------------------------------------------------
#	Leave the rest of the file alone.
# ---------------------------------------------------------------------------

all: $(TARGETS)

TEENSY_LIBS = $(USB_SERIAL_OBJ) $(UART_OBJ) -lcab202_teensy -lprintf_flt -lm 

TEENSY_DIRS =-I$(CAB202_TEENSY_FOLDER) \
	-I$(USB_SERIAL_FOLDER) -L$(CAB202_TEENSY_FOLDER) \
	-I$(UART_FOLDER) -L$(UART_FOLDER)

TEENSY_FLAGS = \
	-std=gnu99 \
	-mmcu=atmega32u4 \
	-DF_CPU=8000000UL \
	-funsigned-char \
	-funsigned-bitfields \
	-ffunction-sections \
	-fpack-struct \
	-fshort-enums \
	-Wall \
	-Werror \
	-Wl,-u,vfprintf \
	-Os 

ZDK_FLAGS = -I$(ZDK_FOLDER) -L$(ZDK_FOLDER) -lzdk -lncurses -lm -Werror -Wall -std=gnu99

clean:
	for f in $(TARGETS); do \
		if [ -f $$f ]; then rm $$f; fi; \
		if [ -f $$f.elf ]; then rm $$f.elf; fi; \
		if [ -f $$f.obj ]; then rm $$f.obj; fi; \
	done

rebuild: clean all

%.hex : %.c
	avr-gcc $< $(TEENSY_FLAGS) $(TEENSY_DIRS) $(TEENSY_LIBS) -o $@.obj
	avr-objcopy -O ihex $@.obj $@

%.exe : %.c
	gcc $< $(ZDK_FLAGS) -o $@
