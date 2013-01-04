GPP_FLAGS := -c -g -O3 -Wall -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=103 -I/usr/share/arduino/hardware/arduino/cores/arduino -I/usr/share/arduino/hardware/arduino/variants/standard


generator.hex: generator.elf
	avr-objcopy -O ihex -R .eeprom generator.elf generator.hex

generator.elf: generator.o main.o wiring.o wiring_analog.o wiring_digital.o
	avr-gcc -O3 -Wl,--gc-sections -mmcu=atmega328p -o generator.elf generator.o main.o wiring.o wiring_analog.o wiring_digital.o -lm

main.o:
	avr-g++ $(GPP_FLAGS) /usr/share/arduino/hardware/arduino/cores/arduino/main.cpp

wiring.o:
	avr-g++ $(GPP_FLAGS) /usr/share/arduino/hardware/arduino/cores/arduino/wiring.c

wiring_analog.o:
	avr-g++ $(GPP_FLAGS) /usr/share/arduino/hardware/arduino/cores/arduino/wiring_analog.c

wiring_digital.o:
	avr-g++ $(GPP_FLAGS) /usr/share/arduino/hardware/arduino/cores/arduino/wiring_digital.c

generator.o: generator.c
	avr-g++ $(GPP_FLAGS) generator.c

upload: generator.hex
	/usr/share/arduino/hardware/tools/avrdude -C/usr/share/arduino/hardware/tools/avrdude.conf -v -patmega328p -carduino -P/dev/ttyACM1 -b115200 -D -Uflash:w:generator.hex:i

clean:
	rm -f *.o *.hex *.elf

