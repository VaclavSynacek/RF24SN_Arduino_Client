#include <SPI.h>
#include "RF24.h"
#include "RF24SN.h"

//setup FR24 driver
RF24 radio(9,10);

//the first parameter is reference to the underlaying driver, see above
//the second parameter is the sensor base pipe address. this has to be the same for all nodes talking to one base
//the third parameter is nodeId. it has to be unique within a network. change it at compile time for each node, or replace with some EEPRROM reading
RF24SN rf24sn(&radio,0xF0F0F0F000LL,3);
bool success;

//here should be variables for application specific HW, sensor and other libraries

int counter=0;
int redPin = 5;
int greenPin = 6;
int bluePin = 7;

void setup(void)
{

  Serial.begin(9600);
  
  //initialize RF24Sensor library (it initializes the underlaying RF24 driver)
  rf24sn.begin();
  rf24sn.printDetails();

  //initialization of application specific HW, sensors and other libraries would be here
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

}

void loop(void)
{

    //do the sensor HW reading here
    
        
        
    Serial.println("----------------------------------");   
    
        Serial.println( counter++ ); 

    Serial.println("----------------------------------"); 
    
    black();
    //try to send the packet 5 times before giving up (usefull for collisions or interferrence)
    success = rf24sn.publish(1,counter,5);
    
    
    if(success)
    {
       green();
       Serial.println("success");
       delay(2000);
    }
    else
    {
      red();
      Serial.println("failure");
      delay(2000);
    }
    
      blue();
      delay(1000);
}

void blue() {
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, HIGH);
}

void red() {
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);
}

void green() {
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, LOW);
}

//needs to be black to limit power supply problems
//on computer power and/or antena-less nRF24L01 usually white works ok
//on battery powered and antena equiped high power nRF24L01
//there are often problems when transmitting and triving all 3 led pins
void black() {
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);
}

