// RF24SN Arduino Client Library Example "Publish Const Value"
// Sends the value of PI to MQTT broker every 5 seconds.
// see https://github.com/VaclavSynacek/RF24SN for general RF24SN description
// see https://github.com/VaclavSynacek/RF24SN_Arduino_Client for library description and new versions
//
// Copyright Vaclav Synacek 2014
// MIT License

#include <SPI.h>
#include "RF24.h"
#include "RF24SN.h"

//RF24 driver:
//see wiring image how to wire up the nRF24L01
#define RF24_CE_PIN 9
#define RF24_CS_PIN 10
//call the constructor
RF24 radio(RF24_CE_PIN, RF24_CS_PIN);

//RF24SN setup:
//each node has to have a unique id within one star topology around one base
#define RF24SN_NODE_ID 1
//all nodes have to know the base address of the base, address has to end with 0 byte
#define RF24SN_SERVER_ADDR 0xF0F0F0F000LL
//each node can have up to 254 sensors to publish to or request from
//this example is using only one sensor
#define CONST_VALUE_SENSOR 1
//call the constructor
RF24SN rf24sn( & radio, RF24SN_SERVER_ADDR, RF24SN_NODE_ID);

void setup(void) {

	Serial.begin(9600);

	//initialize RF24Sensor library (it initializes the underlaying RF24 driver)
	rf24sn.begin();
	rf24sn.printDetails();
}

void loop(void) {

	float constValueToPublish = 3.14;

	Serial.print("Publishing value ");
	Serial.print(constValueToPublish);
	Serial.print(" to MQTT topic RF24SN/in/");
	Serial.print(RF24SN_NODE_ID);
	Serial.print("/");
	Serial.println(CONST_VALUE_SENSOR);

	//publish now
	bool success = rf24sn.publish(CONST_VALUE_SENSOR, constValueToPublish);

	//if it fails often, replace with multiple repeated publish
	//to repeat 10 time use:
	//rf24sn.publish(CONST_VALUE_SENSOR, constValueToPublish, 10);


	//do something if publish was acked by the base or not   
	if (success) Serial.println("success");
	else Serial.println("failure");

	//to save battery replace delay() with proper sleep and use the powerDown/powerUp functions
	//if on mains, don't bother
	//radio.powerDown();
	delay(5000);
	//radio.powerUp();
	//delay(50); //give the radio time to wake up properly
}
