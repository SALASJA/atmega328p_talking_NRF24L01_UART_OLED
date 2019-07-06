currentport=$(ls /dev/cu.wchusbserial*)
avr-gcc -Os -DF_CPU=16000000UL -mmcu=ATmega328P -c -o main.o main.c
avr-gcc -Os -DF_CPU=16000000UL -mmcu=ATmega328P -c -o uart.o ../UART/uart.c
avr-gcc -Os -DF_CPU=16000000UL -mmcu=ATmega328P -c -o SSD1306.o ../SSD1306_C/Files/SSD1306.c
avr-gcc -Os -DF_CPU=16000000UL -mmcu=ATmega328P -c -o TWI.o ../SSD1306_C/Files/TWI.c
avr-gcc -mmcu=ATmega328P TWI.o SSD1306.o main.o uart.o -o main
avr-objcopy -O ihex -R .eeprom main main.hex
avrdude -V -F -p ATmega328P -P $currentport -c stk500v1 -b 115200 -U flash:w:main.hex

