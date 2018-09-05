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

#include "Arduino.h"

enum DeviceID {
  DEVICE_ALLTRAX,
  DEVICE_VESC,
  DEVICE_MOTOR_BOARD,
  DEVICE_BATTERY_BOARD,
  DEVICE_GPS_IMU,
  DEVICE_THROTTLE
};

class TelemetryNode{
  private:
    byte deviceID;
    void pack(byte* packet);
    HardwareSerial *_serial;
    unsigned long timeDelay=1000;
    static const byte PACKET_SIZE=16;
  public:
    TelemetryNode(byte deviceID,HardwareSerial *serialPort); //Constructor
    void streamSerial();
    void begin(long baudrate);
    void setTransmissionRate(float frq);
    byte getChecksum(byte* packet);
};
#endif
