#include <Asysob_GPS.h>

// ***********************************************************************
// GPS_Test - Teensy 3.0 Version
//
// GPS Navilock NL-552ETTL via UART
// +5V, GND, Shield = GND, TX->UART1(RX, Pin 0), RX->UART1(TX, Pin 1)
// ***********************************************************************

Asysob_GPS gps(&Serial1);

boolean gps_fixed;

const int busyPin = 13;

void setup() {
	Serial.begin(115200);
	Serial.println("GPS Test");
  
	// Busy LED on while CPU in loop()
	pinMode(busyPin, OUTPUT);
	
	// Initialize Navilock GPS with default configuration
	gps.Init(38400);
		
	gps_fixed = false;
}

void loop() {
	digitalWrite(busyPin, HIGH); // CPU is busy now
	unsigned long now = millis();
	unsigned long secs = now / 1000;
	Serial.print(secs);
	Serial.println(": loop() again ...");
		
	// Catch any message sent from GPS device as a single string
	char *message = gps.GPSMessage(NULL,false);
	Serial.print("NMEA Sequence: ");
	Serial.println(message);
	
	// Catch any message sent from GPS device as a set of parameters
	message = gps.GPSMessage(NULL,true);
	for (int p=0; p<gps.EntryCount(); p++) {
		Serial.print("Entry ");
		Serial.print(p);
		Serial.print(" -> ");
		Serial.println(gps.MessageEntry(p));
	}

	// Catch next GPRMC message sent from GPS device as a single string
	message = gps.GPSMessage((char *) "GPRMC",false);
	Serial.print("Should be GPRMC Sequence: ");
	Serial.println(message);
	
	Serial.println();
	
	digitalWrite(busyPin, LOW);
	
	// This example reads gps once a second
	delay(1000 - millis() % 1000);
}

