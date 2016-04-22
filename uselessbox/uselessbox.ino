//We always have to include the library
#include "LedControl.h"

enum characters {A,r,d,u,i,n,o};

byte sprite[][5] = {
	{
	B1110,
	B1001,
	B1111,
	B1001,
	B1001
	},

{ B0110,
B1001,
B1111,
B1010,
B1001 },
{ B00011100, B00100010, B00100010, B00010010, B11111110 },
{ B00111100, B00000010, B00000010, B00000100, B00111110 },
{ B00000000, B00100010, B10111110, B00000010, B00000000 },
{ B00111110, B00010000, B00100000, B00100000, B00011110 },
{ B00011100, B00100010, B00100010, B00100010, B00011100 },
};

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 13 is connected to the DataIn
 pin 14 is connected to the CLK
 pin 12 is connected to LOAD
 We have 6 MAX72XX.
 */

LedControl lc = LedControl(13, 14, 12, 6);

/* we always wait a bit between updates of the display */
unsigned long delaytime = 100;
uint32_t buf[16];

void setup() {
	/*
	 The MAX72XX is in power-saving mode on startup,
	 we have to do a wakeup call
	 */
	Serial.begin(9600);
	Serial.println("Hello");
	for (int i = 0; i < 6; i++) {
		lc.shutdown(i, false);

		/* Set the brightness to a medium values */
		lc.setIntensity(i, 8);

		/* and clear the display */
		lc.clearDisplay(i);
	}
}


void loop() {
	writeScreen();
	delay(delaytime);
	for (int i = 0; i < 24; i++) {
		memset(buf, 0x0, 64);
		putSprite(i, 9, 4, 5,  r);
		writeScreen();
		delay(delaytime);
	}
}

// This function works for a single sprite so far WIP
void putSprite(int x, int y, int width, int height,  int sp) {
	for (int i = 0; i < height; i++) {
		buf[i + y] = (uint32_t(sprite[sp][i])) << (23 - x - width);
	}
}


// Write the contents of the buffer.... 
void writeScreen() {
	for (int display = 0; display < 3; display++) {						// Top Row
		for (int row = 0; row < 8; row++) {								// Cycle through row
			lc.setRow(2 - display, row, buf[row] >> (display * 8));		// set leds on top 3 MAX7219s
			lc.setRow(5 - display, row, buf[row + 8] >> (display * 8)); // set leds on bot 3 MAX7219s
		}
	}
}

