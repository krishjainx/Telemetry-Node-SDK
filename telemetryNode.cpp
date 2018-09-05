/*
 * telemetryNode.cpp v1.0
 * Created: 7/10/18 by Andrew Gutierrez
 * Modified: 7/15/18
 *
 * Arduino library for telemetry node for URSS. Library is deisnged to
 * Serve as the serial interface between the various "node" boards in the
 * boat and the telemetry server that collects all the data.
 */

#include <telemetryNode.h>

TelemetryNode::TelemetryNode(byte deviceID, HardwareSerial *serialPort){
  this->deviceID = deviceID;
  _serial=serialPort;
}

void TelemetryNode::begin(long baudrate){
  _serial->begin(baudrate);
 }

 void TelemetryNode::setTransmissionRate(float frq){
   timeDelay=(int)floor((1000/frq));
 }

void TelemetryNode::streamSerial(){
  static byte ndx=0;
  static unsigned long lastTransmission=0;
  static bool isSending=false;
  static byte* packet = new byte[PACKET_SIZE]{0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x00};
  unsigned long currentTime = millis();

  if(isSending){
    _serial->write(packet[ndx]);
    if(ndx>(PACKET_SIZE-2)){
      ndx=0;
      isSending=false;
    }else{
      ndx++;
    }
  }else if(abs(currentTime-lastTransmission)>=timeDelay){
    isSending=true;
    pack(packet);
    lastTransmission=currentTime;
  }
}

void TelemetryNode::pack(byte* packet){
  packet[0]=deviceID;
  switch (deviceID) {
    case DEVICE_ALLTRAX:
      break;
    case DEVICE_VESC:
      break;
    case DEVICE_MOTOR_BOARD:
      break;
    case DEVICE_BATTERY_BOARD:
      break;
    case DEVICE_GPS_IMU:
      break;
    case DEVICE_THROTTLE:
      break;
  }
  packet[15]=getChecksum(packet);
}

byte TelemetryNode::getChecksum(byte* packet){
   byte sum=0;
   for (size_t i = 0; i < PACKET_SIZE-1; i++) {
     sum+=packet[i];
   }
   return 0xFF-sum;
}
