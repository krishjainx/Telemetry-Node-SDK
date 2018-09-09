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

uint8_t _checksum(struct packet *p){
  uint8_t *p8 = (uint8_t*)p;
  uint8_t s = 0;
  for (p8 = (uint8_t*)p; p8 != p+1; ++p8)
    s += *(p8++);
  return s;
}

void TelemetryNode::begin(long baudrate){
  _serial->begin(baudrate);
}

void TelemetryNode::setTransmissionRate(float frq){
  timeDelay=(int)floor((1000/frq));
}

void TelemetryNode::streamSerial(){
  uint8_t index = 0;
  unsigned long lastTransmission = 0;
  bool isSending = false;
  unsigned long currentTime = millis();

  if(isSending){
    if (isSending = (index++ < sizeof(packet)))
      _serial->write(((byte*)p)[index]);
  } else if (currentTime-lastTransmission >= timeDelay){
    isSending=true;
    pack((void*)(&current_pack.data));
    current_pack.device_id = getDeviceID();
    current_pack.packet_n = 0;
    current_pack.checksum = 0;
    current_pack.checksum = _checksum(&current_pack);
    lastTransmission=currentTime;
  }
}

void AlltraxNode::pack(void *p){
  uint16_t* p16 = (uint16_t*)(p);
  p16[0] = diodeTemp;
  p16[1] = inVoltage;
  p16[2] = outCurrent;
  p16[3] = inCurrent;
  uint8_t *p8 = (uint8_t*)(&p16[4]);
  p8[0] = dutyCycle;
  p8[1] = errorCode;
}

void AlltraxNode::readSerial(){
  // read some shit and provide values for vars
  diodeTemp = 0xDE;
  inVoltage = 0xAD;
  outCurrent = 0xBE;
  inCurrent = 0xEF;
  dutyCycle = 0xA;
  errorCode = 0xF;
}

void VescNode::pack(void *p){
  uint16_t *p16 = (uint16_t*)(p);
  p16[0] = fetTemp;
  p16[1] = inVoltage;
  p16[2] = outCurrent;
  p16[3] = inCurrent;
  uint8_t *p8 = (uint8_t*)(&p16[4]);
  p8[0] = dutyCycle;
  p8[1] = faultCode;
}

void VescNode::readSerial(){
  fetTemp = 0xDE;
  inVoltage = 0xAD;
  outCurrent = 0xBE;
  inCurrent = 0xEF;
  dutyCycle = 0xA;
  faultCode = 0xF;
}

void MotorBoardNode::pack(void *p){
  uint32_t *p32 = (uint32_t)(p);
  p32[0] = motorTemp;
  p32[1] = motorRPM;
  p32[2] = propRPM;
}
