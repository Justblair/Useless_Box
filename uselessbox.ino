//We always have to include the library
#include "LedControl.h"

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
unsigned long delaytime = 10;

uint32 buf[16] {0xFFFFFFFF, 0x0, 0xFFFFFFFFFF, 0x0, B11001100 <<16 & B00000000};

void setup() {
	/*
	 The MAX72XX is in power-saving mode on startup,
	 we have to do a wakeup call
	 */
	Serial.begin(9600);
	Serial.println("Hello");
	for (int i = 0; i < 6; i++){
		lc.shutdown(i, false);

		/* Set the brightness to a medium values */
		lc.setIntensity(i, 8);

		/* and clear the display */
		lc.clearDisplay(i);
	}
}

/*
 This method will display the characters for the
 word "Arduino" one after the other on the matrix.
 (you need at least 5x7 leds to see the whole chars)
 */
void writeArduinoOnMatrix() {
	/* here is the data for the characters */
	byte a[] = { B0110,
		B1001,
		B1111,
		B1001,
		B1001 };
	byte r[] = { B0110,
		B1001,
		B1111,
		B1010,
		B1001 };
	byte d[5] = { B00011100, B00100010, B00100010, B00010010, B11111110 };
	byte u[5] = { B00111100, B00000010, B00000010, B00000100, B00111110 };
	byte i[5] = { B00000000, B00100010, B10111110, B00000010, B00000000 };
	byte n[5] = { B00111110, B00010000, B00100000, B00100000, B00011110 };
	byte o[5] = { B00011100, B00100010, B00100010, B00100010, B00011100 };

	/* now display them one by one with a small delay */
	lc.setRow(0, 0, a[0] << 4);
	lc.setRow(0, 1, a[1] << 4);
	lc.setRow(0, 2, a[2] << 4);
	lc.setRow(0, 3, a[3] << 4);
	lc.setRow(0, 4, a[4] << 4);

	lc.setRow(1, 0, a[0]);
	lc.setRow(1, 1, a[1]);
	lc.setRow(1, 2, a[2]);
	lc.setRow(1, 3, a[3]);
	lc.setRow(1, 4, a[4]);
	delay(delaytime);

	lc.setRow(1, 0, r[0]);
	lc.setRow(1, 1, r[1]);
	lc.setRow(1, 2, r[2]);
	lc.setRow(1, 3, r[3]);
	lc.setRow(1, 4, r[4]);
	delay(delaytime);
	lc.setRow(0, 0, d[0]);
	lc.setRow(0, 1, d[1]);
	lc.setRow(0, 2, d[2]);
	lc.setRow(0, 3, d[3]);
	lc.setRow(0, 4, d[4]);
	delay(delaytime);
	lc.setRow(0, 0, u[0]);
	lc.setRow(0, 1, u[1]);
	lc.setRow(0, 2, u[2]);
	lc.setRow(0, 3, u[3]);
	lc.setRow(0, 4, u[4]);
	delay(delaytime);
	lc.setRow(0, 0, i[0]);
	lc.setRow(0, 1, i[1]);
	lc.setRow(0, 2, i[2]);
	lc.setRow(0, 3, i[3]);
	lc.setRow(0, 4, i[4]);
	delay(delaytime);
	lc.setRow(0, 0, n[0]);
	lc.setRow(0, 1, n[1]);
	lc.setRow(0, 2, n[2]);
	lc.setRow(0, 3, n[3]);
	lc.setRow(0, 4, n[4]);
	delay(delaytime);
	lc.setRow(0, 0, o[0]);
	lc.setRow(0, 1, o[1]);
	lc.setRow(0, 2, o[2]);
	lc.setRow(0, 3, o[3]);
	lc.setRow(0, 4, o[4]);
	delay(delaytime);
	lc.setRow(0, 0, 0);
	lc.setRow(0, 1, 0);
	lc.setRow(0, 2, 0);
	lc.setRow(0, 3, 0);
	lc.setRow(0, 4, 0);
	delay(delaytime);
}

/*
  This function lights up a some Leds in a row.
  The pattern will be repeated on every row.
  The pattern will blink along with the row-number.
  row number 4 (index==3) will blink 4 times etc.
  */
void rows() {
	for (int row = 0; row < 8; row++) {
		delay(delaytime);
		lc.setRow(0, row, B10100000);
		delay(delaytime);
		lc.setRow(0, row, (byte)0);
		for (int i = 0; i < row; i++) {
			delay(delaytime);
			lc.setRow(0, row, B10100000);
			delay(delaytime);
			lc.setRow(0, row, (byte)0);
		}
	}
}

void single() {
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			delay(delaytime);
			lc.setLed(0, row, col, true);
			delay(delaytime);
			for (int i = 0; i < col; i++) {
				lc.setLed(0, row, col, false);
				delay(delaytime);
				lc.setLed(0, row, col, true);
				delay(delaytime);
			}
		}
	}
}

void loop() {
	//writeArduinoOnMatrix();
	//rows();	
	writeScreen();
	delay(2000);
	single();
	for (int displ = 0; displ < 6; displ++)	lc.clearDisplay(displ);
	delay(200);

}

void writeScreen(){



	for (int display = 0; display < 3; display++){
		for (int row = 0; row < 8; row++){
			lc.setRow(display, row, buf[row]);
		}
	}
}