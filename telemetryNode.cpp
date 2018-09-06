/*
 * telemetryNode.cpp v1.0
 * Created: 7/10/18 by Andrew Gutierrez
 * Modified: 7/15/18
 *
 * Arduino library for telemetry node for URSS. Library is deisnged to
 * Serve as the serial interface between the various "node" boards in the
 * boat and the telemetry server that collects all the data.
 */

#include "telemetryNode.h"
#include <math.h>

void TelemetryNode::begin(long baudrate){
  _serial->begin(baudrate);
}

void TelemetryNode::setTransmissionRate(float frq){
  timeDelay=(int)floor((1000/frq));
}

void TelemetryNode::streamSerial(){
  byte ndx=0;
  unsigned long lastTransmission=0;
  bool isSending=false;
  struct packet *p= packet_new(DEVICE_ALLTRAX, 0xBEEF, 0x8);
  unsigned long currentTime = millis();

  if(isSending){
    _serial->write((byte*)p, sizeof(packet));
    // don't know why you don't send the last byte
    // send i
    /*
    if(ndx>(PACKET_SIZE-2)){
      ndx=0;
      isSending=false;
    }else{
      ndx++;
    }
    */
  } else if (abs(currentTime-lastTransmission)>=timeDelay){
    isSending=true;
    pack(p);
    lastTransmission=currentTime;
  }
}
