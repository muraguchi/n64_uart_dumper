all: compile upload
compile:
	avr-gcc -o main.elf main.c -mmcu=atmega1284p -Os -g0
	avr-objcopy -I elf32-avr -O ihex main.elf main.hex
	avr-size -C main.elf --mcu=atmega1284p
upload:
	#firmware uploading command HERE for your environment. 
	sudo avrdude -V -c avrispmkII -p atmega1284p -U flash:w:main.hex
	
clean:
	rm -rf main.elf main.hex 
