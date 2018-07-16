/*
 * telemetryNode.h v1.0
 * Created: 7/10/18 by Andrew Gutierrez
 * Modified: 7/15/18
 *
 * Arduino library for telemetry node for URSS. Library is deisnged to
 * Serve as the serial interface between the various "node" boards in the
 * boat and the teleetry server that collects all the data.
 */

#ifndef TELEMETRY_NODE_H
#define TELEMETRY_NODE_H

//Handle compiletime preprocessing serial assignment for board architectures
#if defined(ARDUINO_AVR_LEONARDO)
  #define BOARD_TYPE "Leonardo"
  #define SERIAL_PORT Serial1
#elif defined(ARDUINO_AVR_MEGA2560)
  #define BOARD_TYPE "Mega 2560"
  #define SERIAL_PORT Serial1
#elif defined(ARDUINO_AVR_MICRO)
  #define BOARD_TYPE "Micro"
  #define SERIAL_PORT Serial1
#elif defined(ARDUINO_AVR_MINI)
  #define BOARD_TYPE "Mini"
  #define SERIAL_PORT Serial
#elif defined(ARDUINO_AVR_NANO)
  #define BOARD_TYPE "Nano"
  #define SERIAL_PORT Serial
#elif defined(ARDUINO_AVR_UNO)
    #define BOARD_TYPE "Uno"
    #define SERIAL_PORT Serial
#endif

#include "Arduino.h"

class TelemetryNode{
  private:
    byte deviceID;
    static const int BAUDRATE = 115200;
    static const int PACKET_SIZE = 12;
    static const long PACKET_INTERVAL = 250; //Interval between packets in milliseconds
    byte *packet;
    long lastPacket;
  public:
    TelemetryNode(byte deviceID); //Constructor

    //Getters and setters
    byte getDeviceID();
    byte* getPacket();
    byte getByteAt(byte index);

    //Checksum functions
    byte generateLastByte(byte* dataToSend);
    bool checkSum(byte* recivedData);

    //Serial writing functions
    void setByte(byte index,byte data);
    void setPacket(byte* inPacket);
    void invervalSend();
    void sendPacket();
};
#endif
