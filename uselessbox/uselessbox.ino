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

// MQQT Defines
#define humidity_topic "btsensor/humidity"
#define temperature_topic "btsensor/temperature"
#define heatIndex_topic	"btsensor/heatindex"

#define openhab_temp "/uselessBox/OutTemp"
#define openhab_time "/uselessBox/timeString"
#define openhab_command "/uselessBox/warn"


#define DHTTYPE DHT22   // DHT 11
#define DHTPIN D4     // what pin we're connected to

//#define onoff D3  // We will attach the switch to the top of this...

enum characters { A, r, d, u, i, n, o };

DHT dht(DHTPIN, DHTTYPE);

bool lastSwitchState; // we wil use this for debouncing
unsigned long switchtime = 100; // also used to debounce the switch
unsigned long lastSwitchMillis;
byte motorState = 0; // 0=off, 1=opening, 2=closing

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

const char http_site[] = "192.168.0.23";
const int http_port = 8080;


const int motorA = D0;
const int motorB = D8;
const int topSwitch = D1;
const int botSwitch = D2;

char insideTemp[5];
char outsideTemp[5];
char insideHumidity[5];
char outsideHumidiy[5];


void setup() {
	Serial.begin(115200);
	Serial.println("Hello");

	wakeMAX72XX();

	setup_wifi();
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);

	dht.begin();

	// Set pins for the useless box action
	pinMode(topSwitch, INPUT_PULLUP); // to simplify the circuit, means I can connect straight to the switch
	pinMode(botSwitch, INPUT_PULLUP); // to simplify the circuit, means I can connect straight to the switch	
	pinMode(motorA, OUTPUT);
	pinMode(motorB, OUTPUT);
}



void loop() {
	if (!client.connected()) {
		reconnect();
	}
	client.loop();

	if (oldTime + 60000 < millis()) {  // wait 1 minute between readings
		readDHT();
		oldTime = millis();
	}

	// Traditional Useless box functions...

	// If the top switch is on for any reason, we need to think about closing it
	if (digitalRead(topSwitch) && motorState != 1) {  // Top switch is on
		openBox(random(500, 800));  // Open the box, not too quick though
		//openBox(450);
		if (millis() - lastSwitchMillis < 2000) {
			// switch has been fast toggled, activate new mode
			Serial.println("New Mode");
			client.publish("uselessBox/send" ,"1");

		}
		lastSwitchMillis = millis();
		delay(switchtime); // small delay to debounce the switch
		Serial.print(motorState, DEC);
		Serial.println(" : Open");
		motorState = 1;
	}


	// If the top switch is off and the bottom switch is off 
	// Then the finger has just pressed the button and needs to go home
	else if (!digitalRead(topSwitch) && !digitalRead(botSwitch) && motorState != 2) {
		closeBox(1023);
		delay(switchtime); // small delay to debounce the switch
		Serial.print(motorState, DEC);
		Serial.print(" : Close : ");
		motorState = 2;
		Serial.println(millis() - lastSwitchMillis, DEC);
	}

	// If not the top two, then we need to switch the motors off
	else if (!digitalRead(topSwitch) && digitalRead(botSwitch) && motorState != 0) {
		motorsOff();
		delay(switchtime); // small delay to debounce the switch
		lastSwitchState = LOW;
		motorState = 0;
		Serial.print("total : ");
		Serial.println(millis() - lastSwitchMillis, DEC);
	}
}


void openBox(int armSpeed) {
	analogWrite(motorA, armSpeed);
	analogWrite(motorB, 0);
}

void closeBox(int armSpeed) {
	analogWrite(motorA, 0);
	analogWrite(motorB, armSpeed);
}

void motorsOff() {
	analogWrite(motorA, 0);
	analogWrite(motorB, 0);
}

void readDHT() {
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
	// float hif = dht.computeHeatIndex(f, h);
	// Compute heat index in Celsius (isFahreheit = false)
	float hic = dht.computeHeatIndex(t, h, false);
	client.publish(heatIndex_topic, String(hic).c_str(), true);

	//if (checkBound(t, temp, diff)) {
	temp = t;
	Serial.print("New temperature:");
	Serial.println(String(temp).c_str());
	client.publish(temperature_topic, String(temp).c_str(), true);
	//}

	hum = h;
	Serial.print("New humidity:");
	Serial.println(String(hum).c_str());
	client.publish(humidity_topic, String(hum).c_str(), true);
}

// This is where the MQTT commands are recieved and executed
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

	if (strcmp(topic, openhab_time) == 0) {
		memset(buf, 0, 64);  // Clear the buffer of characters quickly
		for (int i = 0; i < length; i++) {
			//Serial.print(i);
			putChar(rowcount + 2, 8, payload[i]);
			rowcount += arial_8ptDescriptors[int(payload[i]) - 33][0] + 1;
		}
		rowcount = 0;
		for (int i = 0; i < 4; i++) {
			//Serial.print(i);
			putChar(rowcount + 2, 0, outsideTemp[i]);
			rowcount += arial_8ptDescriptors[int(outsideTemp[i]) - 33][0] + 1;
		}
		writeScreen();
	}
	else if (strcmp(topic, openhab_temp) == 0) {
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
	else if (strcmp(topic, openhab_command) == 0) {
		Serial.println("command");
		openBox(800);
		delay(1000);
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

// Place a character into the display buffer 
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
			client.subscribe(openhab_time);
			client.subscribe(openhab_temp);
			client.subscribe(openhab_command);
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


