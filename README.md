RF24SN Arduino Client Library
=============================

Implements the client side of [RF24SN](https://github.com/VaclavSynacek/RF24SN). For full description of protocol, client server setup or alternative implementations, see [RF24SN](https://github.com/VaclavSynacek/RF24SN)


### Installation:
```Shell
cd /path/to/arduino
cd libraries
git clone https://github.com/VaclavSynacek/RF24SN_Arduino_Client.git
```

### Including the library:
all of these includes are required:
```Arduino
#include <SPI.h>
#include "RF24.h"
#include "RF24SN.h"
```

### Setup
update depending on your wiring setup. With the default wiring described underneath this should be ok:
```Arduino
#define RF24SN_NODE_ID 1
#define RF24SN_SERVER_ADDR 0xF0F0F0F000LL
#define RF24_CE_PIN 9
#define RF24_CS_PIN 10

RF24 radio(RF24_CE_PIN, RF24_CS_PIN);
RF24SN rf24sn( &radio, RF24SN_SERVER_ADDR, RF24SN_NODE_ID);

void setup(void) {
	rf24sn.begin();
}
```


### Publish Usage:
To send single float value to MQTT broker use publish function:
```Arduino
#define SENSOR_ID 1  //publishes to RF24SN/in/<RF24SN_NODE_ID>/1
       
float measuredValue = 3.14; //replace this line with actual sensor reading code
bool publishSuccess = rf24sn.publish(SENSOR_ID, measuredValue);
```

### Request Usage:
To request single float value from MQTT broker use request function:
```Arduino
#define SENSOR_ID 2  //request last payload fo RF24SN/out/<RF24SN_NODE_ID>/2
       
float requestValue;
bool requestSuccess = rf24sn.request(SENSOR_ID, &requestValue);
if(requestSuccess)
{
   Serial.println(requestValue); //replace this line with some action code (moving servo, LED on, etc.)
}
