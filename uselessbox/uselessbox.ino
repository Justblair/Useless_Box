//We always have to include the library
#include "DHT.h"
#include "LedControl.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "ascii_4x5.h"
#include "passwords.h"

// Stored in passwords.h
//#define wifi_ssid "ELECTRON-CLUB"
//#define wifi_password ""
//
//#define mqtt_server "81.108.97.190"
//#define mqtt_user ""
//#define mqtt_password ""

#define humidity_topic "btsensor/humidity"
#define temperature_topic "btsensor/temperature"
#define heatIndex_topic	"btsensor/heatindex"

#define DHTTYPE DHT11   // DHT 11
#define DHTPIN D4     // what pin we're connected to

enum characters { A, r, d, u, i, n, o };

DHT dht(DHTPIN, DHTTYPE);

long lastMsg = 0;
float temp = 0.0;
float hum = 0.0;
float diff = 0.2;

unsigned long oldTime;
/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 13 (D7 on Wemos) (yellow wire)	is connected to the DataIn
 pin 14 (D5 on Wemos) (green wire)	is connected to the CLK
 pin 12 (D6 on Wemos) (orange wire)	is connected to LOAD
 We have 6 MAX72XX.
 */

LedControl lc = LedControl(D7, D5, D6, 6);

/* we always wait a bit between updates of the display */
unsigned long delaytime = 100;
uint32_t buf[16];

WiFiClient espClient;
PubSubClient client(espClient);

const int motorAPWM = 15;
const int motorB = 11;
const int switchPin = 9;

char insideTemp[5];
char outsideTemp[5];
char insideHumidity[5];
char outsideHumidiy[5];


void setup() {
	Serial.begin(921600);
	Serial.println("Hello");
	wakeMAX72XX();
	setup_wifi();
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);
	//pinMode(motorAPWM, OUTPUT);
	//pinMode(motorB, OUTPUT);
	//pinMode(switchPin, INPUT_PULLUP);
	//attachInterrupt(switchPin, switchChange, CHANGE);
	dht.begin();
}


void switchChange() {
	// do switchy stuff here

}

void loop() {
	if (!client.connected()) {
		reconnect();
	}
	client.loop();
	//writeScreen();
	//delay(delaytime);
	//for (int i = 0; i < 24; i++) {
	//	char ci[2];
	//	memset(buf, 0x0, 64);
	//	putSprite(i, 9, 4, 5, A);
	//	itoa(i, ci, 10);
	//	putChar(i, 0, 'a');
	//	writeScreen();
	//	delay(delaytime);
	//}
	//putChar(0, 0, 'a');
	//writeScreen();

	if (oldTime + 60000 < millis()) {
		// Reading temperature or humidity takes about 250 milliseconds!
		// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
		float h = dht.readHumidity();
		// Read temperature as Celsius (the default)
		float t = dht.readTemperature();
		// Read temperature as Fahrenheit (isFahrenheit = true)
		float f = dht.readTemperature(true);

		// Check if any reads failed and exit early (to try again).
		if (isnan(h) || isnan(t) || isnan(f)) {
			Serial.println("Failed to read from DHT sensor!");
			return;
		}


		// Compute heat index in Fahrenheit (the default)
		float hif = dht.computeHeatIndex(f, h);
		// Compute heat index in Celsius (isFahreheit = false)
		float hic = dht.computeHeatIndex(t, h, false);
		client.publish(heatIndex_topic, String(hic).c_str(), true);

		//if (checkBound(t, temp, diff)) {
		temp = t;
		Serial.print("New temperature:");
		Serial.println(String(temp).c_str());
		client.publish(temperature_topic, String(temp).c_str(), true);
		//}

		//		if (checkBound(h, hum, diff)) {
		hum = h;
		Serial.print("New humidity:");
		Serial.println(String(hum).c_str());
		client.publish(humidity_topic, String(hum).c_str(), true);

		//	}
		//client.publish(humidity_topic, "Hello");
		Serial.print("Humidity: ");
		Serial.print(h);
		Serial.print(" %\t");
		Serial.print("Temperature: ");
		Serial.print(t);
		Serial.print(" *C ");
		Serial.print(f);
		Serial.print(" *F\t");
		Serial.print("Heat index: ");
		Serial.print(hic);
		Serial.print(" *C ");
		Serial.print(hif);
		Serial.println(" *F");

		// Wait a few seconds between measurements.
		//		delay(2000);
		oldTime = millis();

	}

}

void callback(char* topic, byte* payload, unsigned int length) {
	int rowcount = 0;
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("]");
	for (int i = 0; i < length; i++) {
		char receivedChar = (char)payload[i];
		Serial.print(receivedChar);
	}
	Serial.println();

	if (strcmp(topic, "/uselessBox/timeString") == 0) {
		memset(buf, 0, 64);
		for (int i = 0; i < length; i++) {
			Serial.print(i);
			putChar(rowcount + 2, 8, payload[i]);
			rowcount += arial_8ptDescriptors[int(payload[i]) - 33][0] + 1;
		}
		rowcount = 0;
		for (int i = 0; i < 4; i++) {
			Serial.print(i);
			putChar(rowcount + 2, 0, outsideTemp[i]);
			rowcount += arial_8ptDescriptors[int(outsideTemp[i]) - 33][0] + 1;
		}
		writeScreen();
	}
	else

		if (strcmp(topic, "/uselessBox/owmTemperature") == 0) {
			Serial.print("Temperature: ");
			for (int i = 0; i < 3; i++) {
				outsideTemp[i] = payload[i];
			}
			outsideTemp[3] = 'c';
			for (int i = 0; i < length; i++) {
				char receivedChar = outsideTemp[i];
				Serial.print(receivedChar);
			}
		}


}

void wakeMAX72XX() {
	/*
	 The MAX72XX is in power-saving mode on startup,
	 we have to do a wakeup call
	 */
	for (int i = 0; i < 6; i++) {
		lc.shutdown(i, false);
		/* Set the brightness to a medium values */
		lc.setIntensity(i, 8);
		/* and clear the display */
		lc.clearDisplay(i);
	}
}

void setup_wifi() {
	delay(10);
	// We start by connecting to a WiFi network
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(wifi_ssid);

	WiFi.begin(wifi_ssid, wifi_password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

// This function works for a single sprite so far WIP
void putSprite(int x, int y, int width, int height, int sp) {
	for (int i = 0; i < height; i++) {
		buf[i + y] = (uint32_t(sprite[sp][i])) << (23 - x - width);
	}
}

// This function works for a single sprite so far WIP
void putChar(int x, int y, char sp) {
	int charNo = int(sp) - 32;  // starting point from ascii array
	int width = arial_8ptDescriptors[charNo - 1][0];
	int arrayPos = arial_8ptDescriptors[charNo - 1][1];
	//Serial.print("Array pos: ");
	//Serial.println(arrayPos, DEC);

	for (int i = 0; i < 8; i++) {  // add 8 lines into the buffer
		if (width < 9) {
			if (16 - x >= 0) {
				buf[i + y] ^= arial_8ptBitmaps[arrayPos + i] << (16 - x);
			}
			else {
				buf[i + y] ^= arial_8ptBitmaps[arrayPos + i] >> (x - 16);
			}

		}
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

void reconnect() {
	// Loop until we're reconnected
	while (!client.connected()) {
		Serial.print("Attempting MQTT connection...");
		// Attempt to connect
		// If you do not want to use a username and password, change next line to
		// if (client.connect("ESP8266Client")) {
		if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
			Serial.println("connected");
			client.subscribe("/uselessBox/timeString");
			client.subscribe("/uselessBox/owmTemperature");
		}
		else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}


