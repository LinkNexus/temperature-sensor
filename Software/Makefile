# ── Config ────────────────────────────────────────
MCU     = atmega328p
F_CPU   = 8000000UL
BAUD    = 9600
PORT    = /dev/cu.usbmodem84F7_03C9_1FB81
PROG    = stk500v2
BRATE   = 19200

# ── Tools ─────────────────────────────────────────
CC      = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

# ── Flags ─────────────────────────────────────────
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -O1 -Wall

# ── Sources ───────────────────────────────────────
TARGET  = main
SRC     = main.c ./lib/i2c/I2C.c ./lib/oled/oled.c ./lib/oled/global.c ./dht11/dht11.c ./fan/fan.c ./tach/tach.c

# ── Rules ─────────────────────────────────────────
all: $(TARGET).hex

$(TARGET).elf: $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

flash: $(TARGET).hex
	$(AVRDUDE) -c $(PROG) -p $(MCU) -P $(PORT) -b $(BRATE) -U flash:w:$<:i

fuse:
	$(AVRDUDE) -c $(PROG) -p $(MCU) -P $(PORT) -b $(BRATE) -U lfuse:w:0xe2:m

monitor:
	screen $(PORT) $(BAUD)

clean:
	rm -f $(TARGET).elf $(TARGET).hex

.PHONY: all flash fuse monitor clean
