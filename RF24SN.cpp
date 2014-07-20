/*
  RF24SN.cpp - Alternative network library on top of RF24 library and nRF24l01 radio modules
  Created by Vaclav Synacek, 2014.
  Released under MIT license
*/

#include "Arduino.h"
#include "RF24SN.h"

RF24SN::RF24SN(RF24* radio, uint64_t baseAddress, uint8_t nodeId)
{
    //initialize private variables
    _radio = radio;
    _baseAddress = baseAddress;
    _nodeId = nodeId;
    _nodeAddress = _baseAddress + _nodeId;
}


//initialize underlaying RF24 driver
void RF24SN::begin()
{

    _radio->begin();

    //_radio->setChannel(4);
    //_radio->setDataRate(RF24_250KBPS);
    //_radio->setRetries(0,0);
    
    _radio->setPayloadSize(sizeof(RF24SNPacket));
    _radio->openReadingPipe(1, _nodeAddress);   //pipe for application level ACKs from base. address unique to one node
    _radio->openWritingPipe(_baseAddress);      //pipe for packet broadcasting to base. pipe is shared between all nodes
  
    //ack is done in the RF24SN level, so turn the RF24 level auto-acks off on all pipes.
    _radio->setAutoAck(1, false); 
    _radio->setAutoAck(0, false);  
  
}

bool RF24SN::publish(uint8_t sensorId, float value)
{
  return publish(sensorId, value, 1);
}

bool RF24SN::publish(uint8_t sensorId, float value, int retries)
{
  RF24SNPacket publishPacket = createPacket(PUBLISH_PACKET, sensorId, value);
  RF24SNPacket pubackPacket = sendPacket(publishPacket, retries);
  if (pubackPacket.packetType == PUBACK_PACKET && pubackPacket.value == publishPacket.value && pubackPacket.sensorId == publishPacket.sensorId )
  {
     return true;
  }
  else
  {
    if ( pubackPacket.packetType != NO_PACKET )
    {
      Serial.println("got strangely corrupted puback packet:");
      printPacketDetails(pubackPacket);
    }
    return false;
  }
}


bool RF24SN::request(uint8_t sensorId, float* value)
{
  return request(sensorId, value, 1);
}

bool RF24SN::request(uint8_t sensorId, float* value, int retries)
{
  RF24SNPacket requestPacket = createPacket(REQUEST_PACKET, sensorId, 0);
  RF24SNPacket responsePacket = sendPacket(requestPacket, retries);
  if (responsePacket.packetType == RESPONSE_PACKET && responsePacket.sensorId == requestPacket.sensorId )
  {
     (*value) = responsePacket.value;
     return true;
  }
  else
  {
    if ( responsePacket.packetType != NO_PACKET )
    {
      Serial.println("got strangely corrupted response packet:");
      printPacketDetails(responsePacket);
    }
    return false;
  }
}



//factory method to create new packet to be sent
RF24SNPacket RF24SN::createPacket(uint8_t packetType, uint8_t sensorId, float value)
{
    RF24SNPacket packet;
    packet.packetType = packetType;
    packet.reserved = 255;
    packet.nodeId = _nodeId;
    packet.sensorId = sensorId;
    packet.value = value;
    return packet;
}

//create new "no packet" packet
RF24SNPacket RF24SN::createPacket()
{
    RF24SNPacket packet;
    packet.packetType = NO_PACKET;
    packet.reserved = 0;
    packet.nodeId = _nodeId;
    packet.sensorId = 0;
    packet.value = 0;
    return packet;
}

//send the packet to base, wait for ack-packet received back
RF24SNPacket RF24SN::sendPacket(RF24SNPacket packet)
{
 
		//this will be returned at the end. if no ack packet comes back, then "no packet" packet will be returned    
		RF24SNPacket returnPacket = createPacket();
    

    //send the actual packet
    _radio->write( &packet, sizeof(RF24SNPacket) );
    
    //temporarily overwrite the pipe 0 with address 0
    //this is not intuitive, but otherwise this node would also listen to all packets sent to base
    _radio->openWritingPipe(0);
     
    //start listening for the ack packet from base
    //on pipe 0, there is 0. on pipe 1, there is unique address of this node
    //only packets on pipe 1 should be lisened to.
    _radio->startListening();

    //wait until response is awailable or until timeout
    //the timeout period is random as to minimize repeated collisions.
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    int random_wait_time = random(2000,3000);
      
      //Serial.print("transmission: ");
      //Serial.print(transmission);
      //Serial.print(" random_wait_time: ");
      //Serial.println(random_wait_time);
      
    while ( !_radio->available() && !timeout )
    {
        if (millis() - started_waiting_at > random_wait_time )
        timeout = true;
          
    }

    //if a packet got receive, check it. if nothing recieved, don't bother checking
    if(!timeout)
    {
      //get the actual ack packet received
      _radio->read( &returnPacket, sizeof(RF24SNPacket) );
    }

    //stop listening, so that we can broadcast in the next for cycle
		//this has to be after _radio->read()!
    _radio->stopListening();

    //not intuitive, but before startListening, the pipe 0 was set to 0, so now it needs to be reset to correct address
    _radio->openWritingPipe(_baseAddress); 

    return returnPacket;
}

//send the packet to base, wait for ack-packet received back and check it, optionally resent if ack does not match
RF24SNPacket RF24SN::sendPacket(RF24SNPacket packet, int retries)
{
 
    RF24SNPacket returnPacket = createPacket();
    
    //loop until no retires are left or until successfully acked.
    for(int transmission = 1; (transmission <= retries) && returnPacket.packetType==0 ; transmission++)
    {
				returnPacket = sendPacket(packet);
    }

    return returnPacket;
}

//utility method to print banner on top of RF24 banner
void RF24SN::printDetails()
{
  Serial.println("RF24 Sensor:");
  Serial.print("\tnodeId");
  Serial.println(_nodeId);
  
  //TODO make these work with uint64_t
  //Serial.print("\tbaseAddress");
  //Serial.println(_baseAddress, HEX);
  //Serial.print("\tnodeAddress");
  //Serial.println(_nodeAddress, HEX);
  
  //TODO maybe make this work, but it requies print.h being set up
  //Serial.println("\tOn top of RF24:");
  //_radio->printDetails();
}

//utility method to pretty print packet contents
void RF24SN::printPacketDetails(RF24SNPacket packet)
{
    Serial.print("\tpacketType ");
    Serial.println(packet.packetType);
    Serial.print("\tnodeId ");
    Serial.println(packet.nodeId);
    Serial.print("\tsensorId ");
    Serial.println(packet.sensorId);
    Serial.print("\tvalue ");
    Serial.println(packet.value);
}

  
