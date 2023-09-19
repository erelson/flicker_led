# Name of our project
# (use a single word, e.g. 'myproject')
PROJECT=main

# Source files
SRC=demo.c light_ws2812.c

# Device
DEVICE=attiny85
CLOCK=16000000

# Fuse settings for ATtiny85
# From past notes
LFUSE=0xff
HFUSE=0xdc
EFUSE=0xff
# sets the chip to 16 MHz (and maximum startup time, and brownout vcc=4.3V),
# which needs a corresponding oscillator crystal
#
# Chat GPT's reocmmendations
#LFUSE=0xe2
#HFUSE=0xdf
#EFUSE=0xff

# AVR tools
CC=avr-gcc
OBJCOPY=avr-objcopy
AVRDUDE=avrdude

# Flags
CFLAGS=-DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -Os

# Rules and targets
all: $(PROJECT).hex

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(PROJECT).out: $(SRC:.c=.o)
	$(CC) $(CFLAGS) -o $@ $^

$(PROJECT).hex: $(PROJECT).out
	$(OBJCOPY) -O ihex $< $@

flash: $(PROJECT).hex
	$(AVRDUDE) -c usbtiny -p $(DEVICE) -U flash:w:$(PROJECT).hex

fuse:
	$(AVRDUDE) -c usbtiny -p $(DEVICE) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

clean:
	rm -f $(PROJECT).out $(PROJECT).hex $(SRC:.c=.o)

