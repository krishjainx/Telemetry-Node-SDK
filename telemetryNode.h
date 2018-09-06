/*
 * telemetryNode.h v1.0
 * Created: 7/10/18 by Andrew Gutierrez
 * Modified: 7/15/18
 *
 * Arduino library for telemetry node for URSS. Library is deisnged to
 * Serve as the serial interface between the various "node" boards in the
 * boat and the telemetry server that collects all the data.
 */

#ifndef TELEMETRY_NODE_H
#define TELEMETRY_NODE_H
#define byte unsigned char
#include <stdlib.h>

#include "Arduino.h"

enum DeviceID {
  DEVICE_ALLTRAX,
  DEVICE_VESC,
  DEVICE_MOTOR_BOARD,
  DEVICE_BATTERY_BOARD,
  DEVICE_GPS_IMU,
  DEVICE_THROTTLE
};

// Make a struct so that you don't have to worry about packet size

struct packet {
  byte device_id;
  unsigned long data;
  byte packet_n;
  byte checksum;
};

struct packet *packet_new (byte device_id,
    unsigned long data,
    byte packet_n){
  struct packet *p = (struct packet*)malloc(sizeof(struct packet));
  p->device_id = device_id;
  p->data = data;
  p->packet_n = packet_n;
  p->checksum = 0xFF -device_id + data % 0xFF + packet_n;
  return p;
}

virtual class TelemetryNode{
  private:
    byte deviceID;
    HardwareSerial *_serial;
    unsigned long timeDelay;
    virtual void pack(struct packet *p);
  public:
    TelemetryNode(byte device_ID,HardwareSerial *serialPort)
      : deviceID (device_ID), _serial (serialPort){};
    void streamSerial();
    void begin(long baudrate);
    void setTransmissionRate(float frq);
};

#endif
