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

void TelemetryNode::begin(long baudrate){
  _serial->begin(baudrate);
}

void TelemetryNode::setTransmissionRate(float frq){
  timeDelay=(int)floor((1000/frq));
}

void TelemetryNode::streamSerial(){
  byte index = 0;
  unsigned long lastTransmission = 0;
  bool isSending = false;
  unsigned long currentTime = millis();

  if(isSending){
    if (isSending=(index++ < sizeof(packet)))
      _serial->write(((byte*)p)[index]);
  } else if (currentTime-lastTransmission >= timeDelay){
    isSending=true;
    pack(&current_pack);
    lastTransmission=currentTime;
  }
}

void AlltraxNode::pack(struct packet *p){
  p->device_id = getDeviceID();
  p->packet_n = 0x0; // don't let this last
  float* point = (float*)(p->data);
  *(point++)=diodeTemp;
  *(point++)=inVoltage;
  *(point++)=outCurrent;
  *(point++)=inCurrent;
  byte *point2 = (byte*)(point);
  *(point2++) = dutyCycle;
  p->checksum = p->packet_n;
  for (byte i = 0; i < 13; ++i)
    p->checksum += p->data[i];
}

void AlltraxNode::readSerial(){
  // read some shit and provide values for vars
  diodeTemp = 5.09;
  inVoltage = 5.09;
  outCurrent = 5.09;
  inCurrent = 5.09;
  dutyCycle = 0xD;
}
