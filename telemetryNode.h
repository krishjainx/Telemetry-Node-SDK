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

// Make a struct so that you don't have to worry about packet size

struct packet {
  byte device_id;
  byte data[13];
  byte packet_n;
  byte checksum;
};

virtual class TelemetryNode{
  private:
    // Device ID 
    //   lets transmitted packets be identified to device
    byte deviceID;
    // _serial
    //   the bus through which data is read from sensors
    HardwareSerial *_serial;
    // timeDelay
    //   minimum delay between transmissions
    unsigned long timeDelay;
    // pack
    //   update packet to reflect current values of sensors
    //   p* - packet to update
    virtual void pack(struct packet *p);
    // current_pack
    //   currently available data to be transmitted
    struct packet current_pack;
  public:
    TelemetryNode(byte device_ID,HardwareSerial *serialPort)
      : deviceID (device_ID), _serial (serialPort){
	memset(&current_pack, 0, sizeof(struct packet))};
    // streamSerial
    //   streams at most one byte of current_pack to _serial 
    //   for concurrency purposes
    void streamSerial();
    // readSerial
    //   TODO
    virtual void readSerial();
    // begin
    //   init (maybe we should name this init)
    void begin(long baudrate);


    void setTransmissionRate(float frq);
    byte getDeviceID() const { return deviceID };
};

class AlltraxNode : public TelemetryNode {
  private:
    float diodeTemp; // is it a float?
    float inVoltage;
    float outCurrent;
    float inCurrent;
    byte dutyCycle;
    void pack(struct packet *p)
  public:
      AlltraxNode(HardwareSerial *serialPort)
	: TelemetryNode(DEVICE_ALLTRAX, serialPort){};
      void readSerial();

}

class VescNode : public TelemetryNode {
  private:
    void pack(struct packet *p);
  public:
    MotorBoardNode(HardwareSerial *serialPort)
      : TelemetryNode(DEVICE_VESC, serialPort){};
    void readSerial();
}

class MotorBoardNode : public TelemetryNode {
  private:
    void pack(struct packet *p);
  public:
    MotorBoardNode(HardwareSerial *serialPort)
      : TelemetryNode(DEVICE_MOTOR_BOARD, serialPort){};
    void readSerial();
}

class BatteryBoardNode : public TelemetryNode {
  private:
    void pack(struct packet *p);
  public:
    MotorBoardNode(HardwareSerial *serialPort)
      : TelemetryNode(DEVICE_BATTERY_BOARD, serialPort){};
    void readSerial();
}

class GPSNode : public TelemetryNode {
  private:
    void pack(struct packet *p);
  public:
    MotorBoardNode(HardwareSerial *serialPort)
      : TelemetryNode(DEVICE_GPS_IMU, serialPort){};
    void readSerial();
}

class ThrottleNode : public TelemetryNode {
  private:
    void pack(struct packet *p);
  public:
    MotorBoardNode(HardwareSerial *serialPort)
      : TelemetryNode(DEVICE_THROTTLE, serialPort){};
    void readSerial();
}
#endif
