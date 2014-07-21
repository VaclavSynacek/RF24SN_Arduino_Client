// RF24SN Arduino Client Library Example "Publish and Request"
// Counter connected to MQTT. Request MQTT topic with a number to add (every 30 seconds).
// Periodically adds that number. Publishes to another MQTT topic the sum (every 5 seconds). 
// see https://github.com/VaclavSynacek/RF24SN for general RF24SN description
// see https://github.com/VaclavSynacek/RF24SN_Arduino_Client for library description and new versions
//
// Copyright Vaclav Synacek 2014
// MIT License

#include <SPI.h>
#include "RF24.h"
#include "RF24SN.h"

#define RF24SN_NODE_ID 2
#define RF24SN_SERVER_ADDR 0xF0F0F0F000LL
#define RF24_CE_PIN 9
#define RF24_CS_PIN 10

#define ADD_SENSOR 1
#define COUNT_SENSOR 2

RF24 radio(RF24_CE_PIN, RF24_CS_PIN);
RF24SN rf24sn( &radio, RF24SN_SERVER_ADDR, RF24SN_NODE_ID);

long publishTimer, requestTimer;
float counter, add;

void setup(void) {
	Serial.begin(9600);

	Serial.print("Welcome from RF24SN node ");
	Serial.println(RF24SN_NODE_ID);

	Serial.print("To see stuff, post to MQTT topic RF24SN/out/");
	Serial.print(RF24SN_NODE_ID);
	Serial.print("/");
	Serial.println(ADD_SENSOR);

	Serial.print("Use for example shell command \"mosquitto_pub -m 0.1 -t RF24SN/out/");
	Serial.print(RF24SN_NODE_ID);
	Serial.print("/");
	Serial.print(ADD_SENSOR);
	Serial.println("\"");


	rf24sn.begin();
}

void loop(void) {
	//if it's time to publish then publish
	if (publishTimer + 5000 < millis()) {
		publish();
		publishTimer = millis();
	}

	//if it's time to request then request
	if (requestTimer + 30000 < millis() || requestTimer == 0) {
		request();
		requestTimer = millis();
	}

	//should be some sleep library instead
	delay(1000);
}

//increment the counter, publish result
void publish() {
	counter += add;

	Serial.print("Publishing value ");
	Serial.print(counter);
	Serial.print(" to MQTT topic RF24SN/in/");
	Serial.print(RF24SN_NODE_ID);
	Serial.print("/");
	Serial.println(COUNT_SENSOR);

	bool publishSuccess = rf24sn.publish(COUNT_SENSOR, counter);
	if (publishSuccess) {
		Serial.println("Publish OK");
	} else {
		Serial.println("Publish failed");
	}
}

//request by how much to increment the counter in publish()
void request() {
	Serial.print("Requesting last known alue of MQTT topic RF24SN/out/");
	Serial.print(RF24SN_NODE_ID);
	Serial.print("/");
	Serial.println(ADD_SENSOR);

	bool requestSuccess = rf24sn.request(ADD_SENSOR, &add);

	if (requestSuccess) {
		Serial.print("Got response. From now on, I'll be adding ");
		Serial.println(add);
	} else {
		Serial.println("Request failed");
	}
}