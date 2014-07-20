#include <SPI.h>
#include "RF24.h"
#include "RF24SN.h"

//setup FR24 driver according to HW pin setup
RF24 radio(9,10);

//Setup RF24SN
//the first parameter is reference to the underlaying driver, see above
//the second parameter is the sensor base pipe address. this has to be the same for all nodes talking to one base
//the third parameter is nodeId. it has to be unique within a network. change it at compile time for each node, or replace with some EEPRROM reading
RF24SN rf24sn(&radio,0xF0F0F0F000LL,1);

//here should be variables for application specific HW, sensor and other libraries


void setup(void)
{

  Serial.begin(9600);
  

  //initialize RF24Sensor library (it initializes the underlaying RF24 driver)
  rf24sn.begin();
  rf24sn.printDetails();

  //initialization of application specific HW, sensors and other libraries would be here
  
}

void loop(void)
{
       
    float value = 7;    
        
    Serial.println("----------------------------------");   
        Serial.print("measured value to be sent:");
        Serial.println( value ); 
   
//try to send the the above value as a publish message of sensor 3.
//send it 5 times before giving up (usefull for collisions or interferrence)
    bool success = rf24sn.publish(3, value, 5);
    
    
    if(success)
    {
       Serial.println("success");
     }
    else
    {
      Serial.println("failure");
    }
    
        delay(1500);
//-----------------------------------------------------------------      
//similar as above, but this time we request value of sensor 3.

        
        Serial.println("----------------------------------");   
        Serial.print("request value:");
        
        float retval;
       
//the obtained value will be stored in retval, not returned!
        success = rf24sn.request(3, &retval, 5);
 
     if(success)
    {
      Serial.println(retval);
       Serial.println("success");
     }
    else
    {
      Serial.println("failure");
    }       
     
//to save batteries, replace this with proper atmega sleeping code    
      delay(1500);
}



