MCU ?= atmega328p

ARDUINO_FILES ?= /usr/share/arduino/hardware
ARDUINO_SRCS = $(ARDUINO_FILES)/arduino/cores/arduino

ARDUINO_PORT ?= /dev/ttyACM0


GPP_FLAGS := -c -g -O3 -Wall -fno-exceptions -ffunction-sections -fdata-sections -mmcu=$(MCU) -DF_CPU=16000000L -DARDUINO=101 -I$(ARDUINO_SRCS) -I$(ARDUINO_FILES)/arduino/variants/standard


generator.hex: generator.elf
	avr-objcopy -O ihex -R .eeprom generator.elf generator.hex

generator.elf: generator.o main.o wiring.o wiring_analog.o wiring_digital.o
	avr-gcc -O3 -Wl,--gc-sections -mmcu=$(MCU) -o generator.elf generator.o main.o wiring.o wiring_analog.o wiring_digital.o -lm

main.o:
	avr-g++ $(GPP_FLAGS) $(ARDUINO_SRCS)/main.cpp

wiring.o:
	avr-g++ $(GPP_FLAGS) $(ARDUINO_SRCS)/wiring.c

wiring_analog.o:
	avr-g++ $(GPP_FLAGS) $(ARDUINO_SRCS)/wiring_analog.c

wiring_digital.o:
	avr-g++ $(GPP_FLAGS) $(ARDUINO_SRCS)/wiring_digital.c

generator.o: generator.c
	avr-g++ $(GPP_FLAGS) generator.c

upload: generator.hex
	$(ARDUINO_FILES)/tools/avrdude -C$(ARDUINO_FILES)/tools/avrdude.conf -v -p$(MCU) -carduino -P$(ARDUINO_PORT) -b115200 -D -Uflash:w:generator.hex:i

clean:
	rm -f *.o *.hex *.elf

