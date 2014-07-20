/*
  RF24SN.h - Alternative network library on top of RF24 library and nRF24l01 radio modules
  Created by Vaclav Synacek, February 2013.
  Released under GNU LGPL
*/
#ifndef RF24SN_h
#define RF24SN_h

#include "Arduino.h"
#include "RF24.h"

typedef struct{
  uint8_t packetType;   // 0 reserved, 1=publish, 2=puback, 3=request, 4=response
  uint8_t reserved;
  uint8_t nodeId;      //sensor node id, also used for return address calcualtion
  uint8_t sensorId;    //sensor id
  float value;         //sensor reading
} RF24SNPacket;


class RF24SN
{
  public:
    RF24SN(RF24* radio, uint64_t baseAddress, uint8_t nodeId);
    void begin();
    bool publish(uint8_t sensorId, float value);
    bool publish(uint8_t sensorId, float value, int retries);
    bool request(uint8_t sensorId, float* value);
    bool request(uint8_t sensorId, float* value, int retries);
    void printDetails();

    static const uint8_t NO_PACKET = 0;
    static const uint8_t PUBLISH_PACKET = 1;
    static const uint8_t PUBACK_PACKET = 2;
    static const uint8_t REQUEST_PACKET = 3;
    static const uint8_t RESPONSE_PACKET = 4;

  protected:
    RF24* _radio;
    uint64_t _baseAddress;
    uint8_t _nodeId;
    uint64_t _nodeAddress;
    void printPacketDetails(RF24SNPacket packet);
    RF24SNPacket createPacket(uint8_t type, uint8_t sensorId, float value);
    RF24SNPacket createPacket();
    RF24SNPacket sendPacket(RF24SNPacket packet, int retries);
    RF24SNPacket sendPacket(RF24SNPacket packet);
    
};

#endif
